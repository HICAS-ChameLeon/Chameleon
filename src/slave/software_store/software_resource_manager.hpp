/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：2019-3-6
 * Description：software_resource_manager
 */

#ifndef CHAMELEON_SOFTWARERESOURCEMANAGER_HPP
#define CHAMELEON_SOFTWARERESOURCEMANAGER_HPP
// C++ 11 dependencies
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <map>

#include <stout/json.hpp>
#include <stout/protobuf.hpp>

#include <process/future.hpp>
#include <process/owned.hpp>

#include <process/defer.hpp>
#include <process/id.hpp>
#include <process/future.hpp>
#include <process/process.hpp>
#include <process/subprocess.hpp>

// protobuf
#include <fetcher.pb.h>

#include <chameleon_os.hpp>

using std::string;
using std::unordered_map;
using std::map;
using process::Subprocess;

namespace chameleon{
// Forward declaration
class DownloadProcess;

class SoftwareResourceManager {
public:

    explicit SoftwareResourceManager();
    SoftwareResourceManager(const process::Owned<DownloadProcess>& process);

    /**
     *  download the relevant dependencies of a specified framework
     * @param framework_name
     * @param info protobuf download info
     * @return
     */
    process::Future<Nothing> download(const string& framework_name, const mesos::fetcher::FetcherInfo& info);

    virtual ~SoftwareResourceManager();


    static string m_public_name;

private:
    process::Owned<DownloadProcess> process;

};

/**
 * The genuine actor responsible for downloading dependencies of a specified framework
 */
class DownloadProcess:public process::Process<DownloadProcess>{

public:
    DownloadProcess():ProcessBase(process::ID::generate("downloader")) {

    }

    process::Future<Nothing> download(const string& framework_name, const mesos::fetcher::FetcherInfo& info);

    virtual ~DownloadProcess();

private:

    /**
     * kill the associated pid_ts of "downloader" responsible for downloading for the framework
     * @param framework_name
     */
    void kill(const string& framework_name);

    // key: framework's name, value: the Subproesses of downloaders spawned for this special framework.
    unordered_map<string, pid_t > m_subprocess_pids;
};

}


#endif //CHAMELEON_SOFTWARERESOURCEMANAGER_HPP
