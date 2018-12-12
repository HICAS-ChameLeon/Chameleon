//
// Created by lemaker on 18-12-4.
//

#include "submitter.hpp"
#include "submitter_flags.hpp"

using namespace chameleon;

void Submitter::initialize() {
// Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    DLOG(INFO) << "submitter initializes";
    msp_masterUPID = make_shared<UPID>(UPID(DEFAULT_MASTER));
    JobMessage new_job;
    new_job.set_cpus(1);
    new_job.set_memory(1);
    new_job.set_uuid(m_uuid.toString());
    new_job.set_is_master(true);
    string job_bytes = read_a_file(m_spark_path);
    new_job.set_exe_file(job_bytes);
    send(*msp_masterUPID, new_job);
    LOG(INFO) << "submitted a job to the master " << *msp_masterUPID;
}


int main(int argc, char** argv) {

    chameleon::set_storage_paths_of_glog("submitter");// provides the program name
    chameleon::set_flags_of_glog();

    LOG(INFO) << "glog files paths configuration for submitter finished. OK!";

    chameleon::SubmitterFlagsBase submitterFlagsBase;
    Try<Warnings> load = submitterFlagsBase.load("SUBMITTER", argc, argv);
    submitterFlagsBase.setUsageMessage("Submitterflags");

    if(argc <= 1){
        LOG(INFO) << "Run this program need to set parameters" ;
        LOG(INFO) << submitterFlagsBase.usage();
    } else {
        if (load.isError()) {
            LOG(INFO) << "The input was misformatted";
            LOG(INFO) << submitterFlagsBase.usage();
        } else {
            for(int i = 0;i<argc;i++) {
                string cin_message = argv[i];
                if (cin_message == "--help") {
                    LOG(INFO) << submitterFlagsBase.usage();
                    continue;
                } else {
                    os::setenv("LIBPROCESS_PORT", stringify(submitterFlagsBase.submitter_run_port));
                    process::initialize("submitter");

                    Submitter submitter;
                    submitter.setM_spark_path(submitterFlagsBase.spark_path);
//            LOG(INFO) <<submitterFlagsBase.master_ip_and_port;
                    string master_ip_and_port = "master@" + stringify(submitterFlagsBase.master_ip_and_port);
                    LOG(INFO) << master_ip_and_port;
                    submitter.setDEFAULT_MASTER(master_ip_and_port);

                    PID<Submitter> cur_submitter = process::spawn(submitter);
                    LOG(INFO) << "Running submitter on " << process::address().ip << ":" << process::address().port;

                    const PID<Submitter> submitter_pid = submitter.self();
                    LOG(INFO) << submitter_pid;
//    process::terminate(submitter.self());
                    process::wait(submitter.self());
                }
            }
        }
    }
    return 0;
}

