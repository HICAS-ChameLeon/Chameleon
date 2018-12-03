/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */

#include "master.hpp"
using namespace chameleon;

int main(int argc, char **argv){

    chameleon::set_storage_paths_of_glog("master");// provides the program name
    chameleon::set_flags_of_glog();

    os::setenv("LIBPROCESS_PORT", stringify(6060));
    process::initialize("master");

    Master master;
    PID<Master> cur_master = process::spawn(master);
   LOG(INFO)<< "Running master on " << process::address().ip << ":" << process::address().port;

    const PID<Master> master_pid = master.self();
    LOG(INFO) << master_pid;
//    LOG(ERROR) << "error test";
    process::wait(master.self());
    return 0;
}