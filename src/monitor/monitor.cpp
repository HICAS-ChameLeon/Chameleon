//
// Created by lemaker on 18-11-26.
//

#include "monitor.hpp"
using namespace chameleon;

int main(){
    os::setenv("LIBPROCESS_PORT", stringify(5050));
    process::initialize("monitor");

    Monitor monitor;
    PID<Monitor> cur_monitor = process::spawn(monitor);
    cout << "Running monitor on " << process::address().ip << ":" << process::address().port << endl;
    cout << "PID" << endl;

    const PID<Monitor> monitor_pid = monitor.self();
    cout << monitor_pid << endl;
    process::wait(monitor.self());
    return 0;
}