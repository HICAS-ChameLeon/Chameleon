/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：2019-3-6
 * Description：downloader
 */

#ifndef CHAMELEON_DOWNLOADER_HPP
#define CHAMELEON_DOWNLOADER_HPP

#include <vector>
#include <string>

#include <stout/option.hpp>
#include <stout/result.hpp>
#include <stout/try.hpp>
#include <stout/strings.hpp>
#include <stout/os.hpp>
#include <stout/path.hpp>
#include <stout/net.hpp>
#include <stout/protobuf.hpp>

#include <stout/os/getcwd.hpp>
#include <stout/os/write.hpp>

#include <process/future.hpp>
#include <process/owned.hpp>

#include <process/process.hpp>
#include <process/subprocess.hpp>

// protobuf
#include <fetcher.pb.h>

#include "hdfs.hpp"
#include "status_utils.hpp"

using std::string;
using std::vector;
using process::Subprocess;

namespace chameleon {

    static const string FILE_URI_PREFIX = "file://";
    static const string FILE_URI_LOCALHOST = "file://localhost";
    static const string CACHE_FILE_NAME_PREFIX = "c";

    class Downloader {
    public:
        explicit Downloader() = default;
        /**
         * download the specified files designated in the source_uri to the destination_path.
         * Currently, we support three different kinds of source_uri,
         * 1) "file://", for example, "file:///home/lemaker/abc.tgz"
         * 2)  "http://", for exampke, "http://www.baidu.com/abc.tgz"
         * 3) "hdfs://", for instance, "hdfs://ccrfox246:9000/spark-2.3.0-bin-hadoop2.7.tgz",
         *   note that in 3) it means we have set environment variable "HADOOP_HOME" in the system.
         * @param source_uri
         * @param destination_path make sure that the destiantion_path ended up with the name of the file
         * we want to rename after extracting it.
         * @param frameworks_home
         * @return success or failure message
         */
        static Try<string> download(
                const string& source_uri,
                const string& destination_path,
                const Option<string>& frameworks_home);

        /**
         * extract the specified files designated in the source_uri to the destination_directory
         * currently we support several kinds of file compressed formats,such as tar, tgz, tar.gz, tbz2, tar.bz2, txz, tar.xz
         * @param source_path
         * @param destination_directory
         * @return success of failure message
         */
        static Try<bool> extract(const string& source_path, const string& destination_directory);
        /**
         *
         * @param uri
         * @return the basename of the uri. For example, basename("/home/abc.txt") return "abc.txt"
         */
        static Try<string> basename(const string& uri);

    private:
        /*
         *  // chmod an executable.
         */
        static Try<string> chmod_executable(const string &file_path);
        static Try<string> copy_file(
                const string& source_path,
                const string& destination_path);



        static Result<string> uri_to_local_path(const string& uri, const Option<string>& frameworks_home);
        /**
         * whether is a "http://"
         * @param uri
         * @return
         */
        static bool is_net_uri(const string& uri);
        /**
         * helper function for downloading files through "http://" links.
         * @param source_uri
         * @param destination_path
         * @return
         */
        static Try<string> download_with_net(const string& source_uri, const string& destination_path);
        /**
         * helper function for downloading files from hdfs"
         * @param source_uri
         * @param destination_path
         * @return
         */
        static Try<string> download_with_hadoop_client(const string& source_uri, const string& destination_path);

    };
}

#endif //CHAMELEON_DOWNLOADER_HPP
