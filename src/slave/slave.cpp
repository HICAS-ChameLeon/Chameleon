/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */

#include "slave.hpp"
using namespace chameleon;

void Slave::initialize() {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    msp_masterUPID = make_shared<UPID>(UPID(DEFAULT_MASTER));
    install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);

    HardwareResourcesMessage *hr_message = msp_resource_collector->collect_hardware_resources();
    DLOG(INFO)<< *msp_masterUPID;
    string slave_id = stringify(self().address.ip);
    hr_message->set_slave_id(slave_id);
    DLOG(INFO)<<"before send message to master";

    send(*msp_masterUPID, *hr_message);
    delete hr_message;
    LOG(INFO) << "slave initialize finished ";
}


void Slave::register_feedback(const string& hostname){
    cout<<" receive register feedback from master"<< hostname<<endl;
}

void Slave::finalize() {
    ProcessBase::finalize();
    LOG(INFO)<<"slave finalize()";
}


int main(int argc, char **argv){
    chameleon::set_storage_paths_of_glog("slave");// provides the program name
    chameleon::set_flags_of_glog();

    LOG(INFO) << "glog files paths configuration for slave finished. OK!";

    os::setenv("LIBPROCESS_PORT", stringify(6061));
    process::initialize("slave");

    Slave slave;
    PID<Slave> cur_slave = process::spawn(slave);
    LOG(INFO)<<"Running slave on " << process::address().ip << ":" << process::address().port;


    const PID<Slave> slave_pid = slave.self();
    LOG(INFO)<<slave_pid;
    process::wait(slave.self());
    return 0;
}