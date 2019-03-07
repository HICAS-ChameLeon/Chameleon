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

#include <stout/os/getcwd.hpp>
#include <stout/os/write.hpp>

#include <process/future.hpp>
#include <process/owned.hpp>

#include <process/process.hpp>
#include <process/subprocess.hpp>

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
        static Try<string> download(
                const string& source_uri,
                const string& destination_path,
                const Option<string>& frameworks_home);

        static Try<bool> extract(const string& source_path, const string& destination_directory);


    private:
        static Try<string> chmod_executable(const string &file_path);
        static Try<string> copy_file(
                const string& source_path,
                const string& destination_path);



        static Result<string> uri_to_local_path(const string& uir, const Option<string>& frameworks_home);
        static bool is_net_uri(const string& uri);
        static Try<string> download_with_net(const string& source_uri, const string& destination_path);
        static Try<string> download_with_hadoop_client(const string& source_uri, const string& destination_path);

    };
}

#endif //CHAMELEON_DOWNLOADER_HPP
