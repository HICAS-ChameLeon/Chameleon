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
    install<JobMessage>(&Slave::get_a_job);


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

void Slave::get_a_job(const UPID& master, const JobMessage& job_message){
    LOG(INFO)<<"slave "<<self()<<" got a job";
    const string test_spark_file = path::join(os::getcwd(),"lele_spark-2.3.0.tar");
//    ASSERT_SOME(os::write(test_spark_file,job_message.exe_file()));
    Try<string> decompressed_spark = gzip::decompress(job_message.exe_file());
    if(decompressed_spark.isError()){
        LOG(ERROR)<<"slave got a job file which is not completed or decompressing it had mistakes.";
        LOG(ERROR)<<decompressed_spark.error();
    }else{
        ASSERT_SOME(os::write(test_spark_file,decompressed_spark.get()));
        LOG(INFO)<<"slave "<<self()<<"successfully ungziped a job file";
    }
    const string shell_command = "tar xvf "+test_spark_file;
    Try<Subprocess> s = subprocess(
            shell_command,
            Subprocess::FD(STDIN_FILENO),
            Subprocess::FD(STDOUT_FILENO),
            Subprocess::FD(STDERR_FILENO));
    if(s.isError()){
        LOG(ERROR)<<"slave "<<self()<<"failed to untar the job file.";
        LOG(ERROR)<<s.error();
    }else{
        LOG(INFO)<<"slave "<<self()<<"successfully untar a job file";
    }
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