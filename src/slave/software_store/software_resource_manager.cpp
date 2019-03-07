/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：2019-3-6
 * Description：software_resource_manager
 */

#include "software_resource_manager.hpp"
namespace chameleon{

    SoftwareResourceManager::SoftwareResourceManager() {

    }

    SoftwareResourceManager::~SoftwareResourceManager(){

    }

    process::Future<Nothing> DownloadProcess::download(const string &framework_name, const mesos::fetcher::FetcherInfo& info) {

        const string stdoutPath = path::join(info.sandbox_directory(), "stdout");

        Try<int_fd> out = os::open(
                stdoutPath,
                O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK | O_CLOEXEC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        if (out.isError()) {
            return process::Failure("Failed to create 'stdout' file: " + out.error());
        }

        string stderrPath = path::join(info.sandbox_directory(), "stderr");
        Try<int_fd> err = os::open(
                stderrPath,
                O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK | O_CLOEXEC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        if (err.isError()) {
            os::close(out.get());
            return process::Failure("Failed to create 'stderr' file: " + err.error());
        }

//        string downloader_path = path::join(
        string downloader_path = path::join(setting::SLAVE_EXE_DIR,"/software_store/downloader");
        LOG(INFO)<<"downloader_path "<<downloader_path;

        map<string,string> environment;
        environment["CHAMELEON_FETCHER_INFO"] = stringify(JSON::protobuf(info));
        environment["framework_name"] = framework_name;
        LOG(INFO)<<"lele download dependencies of framework_name: "<<framework_name;
        Try<Subprocess> download_subprocess = process::subprocess(
                downloader_path,
                Subprocess::PIPE(),
                Subprocess::FD(out.get(), Subprocess::IO::OWNED),
                Subprocess::FD(err.get(), Subprocess::IO::OWNED),
                environment
                );

        if(download_subprocess.isError()){
            return process::Failure("Failed to execute downloader:"+download_subprocess.error());
        }

        m_subprocess_pids[framework_name]=download_subprocess.get().pid();
        return download_subprocess.get().status().then(process::defer(self(),[=](const Option<int>& status) -> process::Future<Nothing>{
            if(status.isNone()){
                return process::Failure("No status available from chameleon-downloader");
            }

            if(status.get()!=0){
                return process::Failure("Failed to download all urils for framework name:"+framework_name + "with exit status:"+stringify(status.get()));
            }
            return Nothing();
        }))
        .onFailed(process::defer(self(), [=](const string&){

            Try<string> text = os::read(stderrPath);
            if(text.isSome()){
                LOG(WARNING) << "Begin fetcher log (stderr in sandbox) for framework name "
                             << framework_name << " from running command: " << downloader_path
                             << "\n" << text.get() << "\n"
                             << "End fetcher log for container " << framework_name;
            }else{
                LOG(ERROR) << "download log (stderr in sandbox) for framework name "
                           << framework_name << " not readable: " << text.error();
            }
        }))
        .onAny(process::defer(self(),[=](const process::Future<Nothing>&){
            LOG(INFO)<<"clear the subprocess PID remembere from running chameleon-downloader";
            m_subprocess_pids.erase(framework_name);
        }));
    }

    void DownloadProcess::kill(const string &framework_name) {
        LOG(INFO)<<"Killing the downloader for the framework "<<framework_name;

        // Best effort kill the entire downloader tree.
        os::killtree(m_subprocess_pids.at(framework_name), SIGKILL);
        m_subprocess_pids.erase(framework_name);
    }

    DownloadProcess::~DownloadProcess() {
        for(auto it=m_subprocess_pids.begin();it!= m_subprocess_pids.end();it++){
            kill(it->first);
        }
    }
}

using namespace chameleon;
int main(){
    return 0;
}
