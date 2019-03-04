//
// Created by marcie on 19-2-24.
//

#include <super_master_related.pb.h>
#include "super_master.hpp"

namespace chameleon {
    void SuperMaster::initialize() {
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        //install<OwnedSlavesMessage>(&SuperMaster::terminating_master);

        install<OwnedSlavesMessage>(&SuperMaster::kill_master_message);
        // change from two level to one levels
        m_masters_size = 2;
        m_uuid = UUID::random().toString();
        m_first_to_second_master = "super_master@172.20.110.141:7000";

        UPID t_master(m_first_to_second_master);
        send(t_master, "KILL");
        LOG(INFO) << "sends a 'KILL' to a super_master: " << m_first_to_second_master;

    }

    void SuperMaster::kill_master_message(const UPID &from, const OwnedSlavesMessage &message){
        LOG(INFO) << "MAKUN RECEIVED ownedSlavesMessage";
        LOG(INFO) << " get an OwnedSlavesMessage from " << from;
        LOG(INFO) << "MAKUN slaveInfo size: " << message.slave_infos().size();

        std::copy(message.slave_infos().begin(), message.slave_infos().end(), std::back_inserter(m_admin_slaves));
        LOG(INFO) << "MAKUN admin slave size: " << m_admin_slaves.size();
        for (SlaveInfo &slaveInfo: m_admin_slaves) {
            LOG(INFO) << "MAKUN slaveInfo has " << slaveInfo.hardware_resources().cpu_collection().cpu_infos_size() << "CPU";
        }
    }



}
using namespace chameleon;


int main(int argc, char **argv) {
    chameleon::set_storage_paths_of_glog("kill_master");// provides the program name
    chameleon::set_flags_of_glog();

    os::setenv("LIBPROCESS_PORT", "7001");
    process::initialize("kill_master");

    SuperMaster super_master;
    PID<SuperMaster> cur_super_master = process::spawn(super_master);

    LOG(INFO) << "Running kill_master on " << process::address().ip << ":" << process::address().port;

    const PID<SuperMaster> super_master_pid = super_master.self();
//    LOG(INFO) << super_master_pid;

    process::wait(super_master.self());


    return 0;
}
