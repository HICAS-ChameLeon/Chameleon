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
    std::cout << *msp_masterUPID << std::endl;
    string slave_id = stringify(self().address.ip);
    hr_message->set_slave_id(slave_id);
    cout<<"before send "<<endl;

    send(*msp_masterUPID, *hr_message);
    delete hr_message;
}


void Slave::register_feedback(const string& hostname){
    cout<<" receive register feedback from master"<< hostname<<endl;
}

void Slave::finalize() {
    ProcessBase::finalize();
    std::cout<<"slave finalize()"<<std::endl;
}


int main(){
    os::setenv("LIBPROCESS_PORT", stringify(6061));
    process::initialize("slave");

    Slave slave;
    PID<Slave> cur_slave = process::spawn(slave);
    cout << "Running slave on " << process::address().ip << ":" << process::address().port << endl;
    cout << "PID" << endl;

    const PID<Slave> slave_pid = slave.self();
    cout << slave_pid << endl;
    process::wait(slave.self());
    return 0;
}