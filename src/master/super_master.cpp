/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-1-15
 * Description：super_master
 */

#include "super_master.hpp"
namespace chameleon {
    void SuperMaster::initialize(){
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        install<MasterRegisteredMessage>(&SuperMaster::registered_master);
    }

    void SuperMaster::registered_master(const UPID &from, const MasterRegisteredMessage &master_registered_message){
        Future<bool> distinctive = true;
        distinctive = distinctive.then(defer(self(),&Self::is_repeated_registered,from));
        distinctive.onAny(defer(self(),&Self::record_master,lambda::_1,master_registered_message));

    }

    Future<bool> SuperMaster::is_repeated_registered(const UPID& upid){
        if(std::find(m_nodes.begin(),m_nodes.end(),upid)!=m_nodes.end()){
            LOG(INFO)<<" master "<<upid<<" registered repeatedly!";

            return false;
        }
        return true;
    }

    void SuperMaster::record_master(const Future<bool>& future,const MasterRegisteredMessage &master_registered_message){
        CHECK(!future.isDiscarded());
        if(!future.isReady()){
            LOG(ERROR)<<"Failed to record master for this super master due to "<<(future.isFailed() ? future.failure() : "future discarded");
            return;
        }
        if(!future.get()){
            LOG(INFO)<<" master registered repeatedly!";
            return;
        }

    }


}
using namespace chameleon;


int main(int argc, char **argv) {
            chameleon::set_storage_paths_of_glog("super_master");// provides the program name
            chameleon::set_flags_of_glog();

            os::setenv("LIBPROCESS_PORT", "7000");
            process::initialize("super_master");

            SuperMaster super_master;
            PID<SuperMaster> cur_super_master = process::spawn(super_master);
            LOG(INFO) << "Running super_master on " << process::address().ip << ":" << process::address().port;

            const PID<SuperMaster> super_master_pid = super_master.self();
            LOG(INFO) << super_master_pid;
            process::wait(super_master.self());


    return 0;
}
