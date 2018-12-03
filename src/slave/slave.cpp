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
    google::InitGoogleLogging(argv[0]);
    google::SetLogDestination(google::GLOG_FATAL, "../log/log_fatal_"); // set the storage path and prefix of google::FATAL
    google::SetLogDestination(google::GLOG_ERROR, "../log/log_error_");
    google::SetLogDestination(google::GLOG_WARNING, "../log/log_warning_");
    google::SetLogDestination(google::GLOG_INFO, "../log/log_info_");

    FLAGS_alsologtostderr = true;  // glog print to glog files as well as standard input,output, error output
    FLAGS_colorlogtostderr = true;  // configure the color support
    FLAGS_log_prefix = true;  // the prefix should print at the beginning of each row
    FLAGS_logbufsecs = 0;  // output the cache immediately, no cache
    FLAGS_max_log_size = 10;  // set the maximum size of the glog file (unit is MB)
    FLAGS_stop_logging_if_full_disk = true;  // disable the output of glog if the disk is full or the disk has no free space

    LOG(INFO) << "glog path configuration finished. OK!";


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