//
// Created by lemaker on 18-11-26.
//

#include "slave.hpp"
using namespace chameleon;

int main(){
    os::setenv("LIBPROCESS_PORT", stringify(5051));
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