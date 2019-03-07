/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：2019-3-6
 * Description：downloader
 */

#include "downloader.hpp"

namespace chameleon {
    // chmod an executable. For example, we could define some static flags
// so that someone can do: os::chmod(path, EXECUTABLE_CHMOD_FLAGS).
    Try<string> Downloader::chmod_executable(const string &filePath) {
        Try<Nothing> chmod = os::chmod(
                filePath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if (chmod.isError()) {
            return Error("Failed to chmod executable '" +
                         filePath + "': " + chmod.error());
        }

        return filePath;
    }

    Try<string> Downloader::copy_file(
            const string &source_path,
            const string &destination_path) {
        int status = os::spawn("cp", {"cp", source_path, destination_path});

        if (status == -1) {
            return ErrnoError("Failed to copy '" + source_path + "'");
        }

        if (!WSUCCEEDED(status)) {
            return Error(
                    "Failed to copy '" + source_path + "': " + WSTRINGIFY(status));
        }

        LOG(INFO) << "Copied resource '" << source_path
                  << "' to '" << destination_path << "'";

        return destination_path;
    }

    Result<string> Downloader::uri_to_local_path(const string &uri, const Option<string> &frameworks_home) {
        if (!strings::startsWith(uri, FILE_URI_PREFIX) &&
            strings::contains(uri, "://")) {
            return None();
        }

        string path = uri;
        bool fileUri = false;

        if (strings::startsWith(path, FILE_URI_LOCALHOST)) {
            path = path.substr(FILE_URI_LOCALHOST.size());
            fileUri = true;
        } else if (strings::startsWith(path, FILE_URI_PREFIX)) {
            path = path.substr(FILE_URI_PREFIX.size());
            fileUri = true;
        }

#ifndef __WINDOWS__
        const bool is_relative_path = !strings::startsWith(path, "/");

        if (is_relative_path) {
            if (fileUri) {
                return Error("File URI only supports absolute paths");
            }

            if (frameworks_home.isNone() || frameworks_home.get().empty()) {
                return Error("A relative path was passed for the resource but the "
                             "Mesos framework home was not specified. "
                             "Please either provide this config option "
                             "or avoid using a relative path");
            } else {
                path = path::join(frameworks_home.get(), path);
                LOG(INFO) << "Prepended Mesos frameworks home to relative path, "
                          << "making it: '" << path << "'";
            }
        }
#endif // __WINDOWS__

        return path;

    }

    bool Downloader::is_net_uri(const string &uri) {
        return strings::startsWith(uri, "http://") ||
               strings::startsWith(uri, "https://") ||
               strings::startsWith(uri, "ftp://") ||
               strings::startsWith(uri, "ftps://");
    }

    Try<string> Downloader::download_with_net(const string &source_uri, const string &destination_path) {
        LOG(INFO) << "Downloading resource from '" << source_uri
                  << "' to '" << destination_path << "'";

        Try<int> code = net::download(source_uri, destination_path);
        if (code.isError()) {
            return Error("Error downloading resource: " + code.error());
        } else {
            // The status code for successful HTTP requests is 200, the status code
            // for successful FTP file transfers is 226.
            if (strings::startsWith(source_uri, "ftp://") ||
                strings::startsWith(destination_path, "ftps://")) {
                if (code.get() != 226) {
                    return Error("Error downloading resource, received FTP return code " +
                                 stringify(code.get()));
                }
            } else {
                if (code.get() != 200) {
                    return Error("Error downloading resource, received HTTP return code " +
                                 stringify(code.get()));
                }
            }
        }

        return destination_path;
    }

    Try<string> Downloader::download_with_hadoop_client(const string& source_uri, const string& destination_path){
        Try<process::Owned<HDFS>> hdfs = HDFS::create();
        if (hdfs.isError()) {
            return Error("Failed to create HDFS client: " + hdfs.error());
        }

        LOG(INFO) << "Downloading resource with Hadoop client from '" << source_uri
                  << "' to '" << destination_path << "'";

        process::Future<Nothing> result = hdfs.get()->copy_to_local(source_uri, destination_path);
        result.await();

        if (!result.isReady()) {
            return Error("HDFS copyToLocal failed: " +
                         (result.isFailed() ? result.failure() : "discarded"));
        }

        return destination_path;
    }

    Try<string> Downloader::download(
            const string &source_uri,
            const string &destination_path,
            const Option<string> &frameworks_home) {
        const string _source_uri = strings::trim(source_uri, strings::PREFIX);
        LOG(INFO) << "lele Fetching URI '" << _source_uri << "'";

        // 1. Try to fetch using a local copy.
        // We regard as local: "file://" or the absence of any URI scheme.
        Result<string> source_path = uri_to_local_path(source_uri, frameworks_home);

        if (source_path.isError()) {
            return Error(source_path.error());
        } else if (source_path.isSome()) {
            return copy_file(source_path.get(), destination_path);
        }

        // 2. Try to fetch URI using os::net / libcurl implementation.
        // We consider http, https, ftp, ftps compatible with libcurl.
        if (is_net_uri(source_uri)) {
            return download_with_net(source_uri, destination_path);
        }

        // 3. Try to fetch the URI using hadoop client.
        // We use the hadoop client to fetch any URIs that are not
        // handled by other fetchers(local / os::net). These URIs may be
        // `hdfs://` URIs or any other URI that has been configured (and
        // hence handled) in the hadoop client. This allows mesos to
        // externalize the handling of previously unknown resource
        // endpoints without the need to support them natively.
        // Note: Hadoop Client is not a hard dependency for running mesos.
        // This allows users to get mesos up and running without a
        // hadoop_home or the hadoop client setup but in case we reach
        // this part and don't have one configured, the fetch would fail
        // and log an appropriate error.
        return download_with_hadoop_client(source_uri, destination_path);
    }

    Try<bool> Downloader::extract(const string& source_path, const string& destination_directory){
        Try<Nothing> result = Nothing();

        Option<Subprocess::IO> in = None();
        Option<Subprocess::IO> out = None();
        vector<string> command;

        // Extract any .tar, .tgz, tar.gz, tar.bz2 or zip files.
        if (strings::endsWith(source_path, ".tar") ||
            strings::endsWith(source_path, ".tgz") ||
            strings::endsWith(source_path, ".tar.gz") ||
            strings::endsWith(source_path, ".tbz2") ||
            strings::endsWith(source_path, ".tar.bz2") ||
            strings::endsWith(source_path, ".txz") ||
            strings::endsWith(source_path, ".tar.xz")) {
            command = {"tar", "-C", destination_directory, "-xf", source_path};
        } else if (strings::endsWith(source_path, ".gz")) {
            string pathWithoutExtension = source_path.substr(0, source_path.length() - 3);
            string filename = Path(pathWithoutExtension).basename();
            string destinationPath = path::join(destination_directory, filename);

            command = {"gunzip", "-d", "-c"};
            in = Subprocess::PATH(source_path);
            out = Subprocess::PATH(destinationPath);
        } else if (strings::endsWith(source_path, ".zip")) {
            command = {"unzip", "-o", "-d", destination_directory, source_path};
        } else {
            return false;
        }

        CHECK_GT(command.size(), 0u);

        Try<Subprocess> extractProcess = subprocess(
                command[0],
                command,
                in.getOrElse(Subprocess::PATH(os::DEV_NULL)),
                out.getOrElse(Subprocess::FD(STDOUT_FILENO)),
                Subprocess::FD(STDERR_FILENO));

        if (extractProcess.isError()) {
            return Error(
                    "Failed to extract '" + source_path + "': '" +
                    strings::join(" ", command) + "' failed: " +
                    extractProcess.error());
        }

        // `status()` never fails or gets discarded.
        int status = extractProcess->status()->get();
        if (!WSUCCEEDED(status)) {
            return Error(
                    "Failed to extract '" + source_path + "': '" +
                    strings::join(" ", command) + "' failed: " +
                    WSTRINGIFY(status));
        }

        LOG(INFO) << "Extracted '" << source_path << "' into '"
                  << destination_directory << "'";

        return true;
    }


}

using namespace chameleon;
int main() {
    Downloader downloader;
    const string source_uri = "http://mirrors.hust.edu.cn/apache/spark/spark-2.4.0/spark-2.4.0-bin-hadoop2.7.tgz";
//    const string source_uri = "https://www-eu.apache.org/dist/spark/spark-2.4.0/spark-2.4.0-bin-hadoop2.7.tgz";
//    const string source_uri = "file:///home/lemaker/open-source/spark-2.3.0-bin-hadoop2.7.tgz";
    const string destination_path = "/home/lemaker/open-source/Chameleon/build/spark.tgz";
    Try<string> result = downloader.download(source_uri,destination_path,"/home/lemaker");
    if(result.isError()){
        std::cout<<result.get()<<std::endl;
    }

    Try<bool> result_extract = downloader.extract("/home/lemaker/open-source/Chameleon/build/spark.tgz", "/home/lemaker/open-source/Chameleon/build/");
    if(result_extract.isError()){
        std::cout<<result_extract.get()<<std::endl;
    }

    return 0;
}