//
// Created by lemaker on 18-11-26.
//

#include "master.hpp"
using namespace chameleon;

int main(){
    os::setenv("LIBPROCESS_PORT", stringify(5050));
    process::initialize("master");

    Master master;
    PID<Master> cur_master = process::spawn(master);
    cout << "Running master on " << process::address().ip << ":" << process::address().port << endl;
    cout << "PID" << endl;

    //get slave ip and port
    string slave_str;
    std::cin >> slave_str;
    UPID slaveUPID(slave_str);
    master.slave = slaveUPID;


    const PID<Master> master_pid = master.self();
    cout << master_pid << endl;
    process::wait(master.self());
    return 0;
}