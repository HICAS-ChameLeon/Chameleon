/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：2019-3-6
 * Description：software_resource_manager
 */

#include "software_resource_manager.hpp"
namespace chameleon{

    SoftwareResourceManager::SoftwareResourceManager(const string& slave_path_, const string& public_resources_):m_public_resources(public_resources_),m_slave_path(slave_path_),process(new DownloadProcess(public_resources_)) {
        initialize();
    }

    SoftwareResourceManager::SoftwareResourceManager(const process::Owned<DownloadProcess>& _process):process(_process){
        process::spawn(process.get());
    }

    SoftwareResourceManager::~SoftwareResourceManager(){
        LOG(INFO)<< "~SoftwareResourceManager";
        process::terminate(process.get());
        process::wait(process.get());
    }

    void SoftwareResourceManager::initialize() {
        Try<Nothing> public_resources_create = os::mkdir(m_public_resources);
        if(public_resources_create.isError()){
            LOG(FATAL)<<public_resources_create.error();
            return;
        }else{
            process::spawn(process.get());
        }
    }

    process::Future<Nothing> SoftwareResourceManager::download(const string &framework_name,
                                                               const mesos::fetcher::FetcherInfo &info) {
        return process::dispatch(process.get(),&DownloadProcess::download,framework_name,info);
    }

    process::Future<Nothing> DownloadProcess::download(const string &framework_name, const mesos::fetcher::FetcherInfo& info) {

        LOG(INFO)<<"DownloadProcess::download ";

        const string stdoutPath = path::join(info.sandbox_directory(), "stdout");

        Try<int_fd> out = os::open(
                stdoutPath,
                O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK | O_CLOEXEC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        if (out.isError()) {
            return process::Failure("Failed to create 'stdout' file: " + stdoutPath + " . " +out.error());
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
        string downloader_path = path::join(os::getcwd(),"/software_store/downloader");
//        string downloader_path = path::join("/home/lemaker/open-source/Chameleon/build/src/slave","/software_store/downloader");
        LOG(INFO)<<"downloader_path "<<downloader_path;

        map<string,string> environment;
        environment["CHAMELEON_FETCHER_INFO"] = stringify(JSON::protobuf(info));
        environment["framework_name"] = framework_name;
        environment["PUBLIC_RESOURCES_DIR"] = m_public_resources_dir;
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
            if(!it->first.empty()){
                kill(it->first);
            }
        }
    }
}

//using namespace chameleon;
//int main(){
//
//    SoftwareResourceManager manager;
//    mesos::fetcher::FetcherInfo* fetcher_info = new mesos::fetcher::FetcherInfo();
//    mesos::fetcher::FetcherInfo_Item* item = fetcher_info->add_items();
//    mesos::fetcher::URI* uri = new mesos::fetcher::URI();
//    uri->set_value("http://mirrors.hust.edu.cn/apache/spark/spark-2.3.2/spark-2.3.2-bin-hadoop2.7.tgz");
//    item->set_allocated_uri(uri);
//    item->set_action(mesos::fetcher::FetcherInfo_Item_Action_BYPASS_CACHE);
//    fetcher_info->set_sandbox_directory("/home/lemaker/open-source/Chameleon/build/sanbox");
//    process::Future<Nothing> result = manager.download("my_spark",*fetcher_info);
//    while(result.isPending()){
//
//    }
//    if(result.isFailed()){
//        std::cout<<result.failure();
//    }else if(result.isReady()){
//        std::cout<<"downloaded successfully";
//    }
//
//
//    delete fetcher_info;
//
//    return 0;
//}
