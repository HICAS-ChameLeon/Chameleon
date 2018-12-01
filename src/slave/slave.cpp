/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */

#include "slave.hpp"
using namespace chameleon;

void Slave::initialize() {

    mp_masterUPID = new UPID(DEFAULT_MASTER);
    install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);

    HardwareResourcesMessage hr_message = msp_resource_collector->collect_hardware_resources();
    std::cout<<*mp_masterUPID<<std::endl;
    string slave_id = self().id;
    hr_message.set_allocated_slave_id(&slave_id);
    cout<<"before send "<<endl;

    send(*mp_masterUPID,hr_message);
    int a=456;
    cout<<"a = 456 "<<endl;
}


void Slave::register_feedback(const string& hostname){
    cout<<" receive register feedback from master"<< hostname<<endl;
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