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

    const PID<Master> master_pid = master.self();
    cout << master_pid << endl;
    process::wait(master.self());
    return 0;
}