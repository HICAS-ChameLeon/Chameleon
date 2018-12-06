//
// Created by lemaker on 18-12-4.
//

#include "submitter.hpp"

using namespace chameleon;

void Submitter::initialize() {
// Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    DLOG(INFO)<<"submitter initializes";
    msp_masterUPID = make_shared<UPID>(UPID(DEFAULT_MASTER));
    JobMessage new_job;
    new_job.set_cpus(1);
    new_job.set_memory(1);
    new_job.set_uuid(m_uuid.toString());
    new_job.set_is_master(true);
    string job_bytes = read_a_file(m_spark_path);
    new_job.set_exe_file(job_bytes);
    send(*msp_masterUPID,new_job);
    LOG(INFO)<<"submitted a job to the master "<<*msp_masterUPID;
}

int main(){
    chameleon::set_storage_paths_of_glog("submitter");// provides the program name
    chameleon::set_flags_of_glog();

    LOG(INFO) << "glog files paths configuration for submitter finished. OK!";

    os::setenv("LIBPROCESS_PORT", stringify(6062));
    process::initialize("submitter");

    Submitter submitter;
    PID<Submitter> cur_submitter = process::spawn(submitter);
    LOG(INFO)<<"Running submitter on " << process::address().ip << ":" << process::address().port;


    const PID<Submitter> submitter_pid = submitter.self();
    LOG(INFO)<<submitter_pid;
//    process::terminate(submitter.self());
    process::wait(submitter.self());
    return 0;
}

