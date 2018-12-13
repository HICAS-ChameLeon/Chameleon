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


int main(int argc, char **argv) {

    chameleon::set_storage_paths_of_glog("submitter");// provides the program name
    chameleon::set_flags_of_glog();

    LOG(INFO) << "glog files paths configuration for submitter finished. OK!";

    chameleon::SubmitterFlagsBase flags;
    Try<Warnings> load = flags.load("SUBMITTER", argc, argv);
    flags.setUsageMessage("Submitterflags");

    if (flags.help == 1) {
        LOG(INFO) << "How to run this: " << flags.usage();
    } else {
        if (flags.master_ip_and_port.empty()&& flags.spark_path.empty() && flags.submitter_run_port.empty()) {
            EXIT(EXIT_FAILURE)
                    << "To run this program,must set all parameters and correctly \n"
                       "please check you input or use --help ";
        }
        if (flags.master_ip_and_port.empty()) {
            EXIT(EXIT_FAILURE)
                    << "masterinfo invalid value , see --masterinfo flag";
        }
        if (flags.spark_path.empty()) {
            EXIT(EXIT_FAILURE)
                    << "masterinfo invalid value , see --spath flag";
        }
        if (flags.submitter_run_port.empty()) {
            EXIT(EXIT_FAILURE)
                    << "masterinfo invalid value , see --port flag";
        } else {
            if (!flags.master_ip_and_port.empty() && !flags.spark_path.empty() && !flags.submitter_run_port.empty()) {
                os::setenv("LIBPROCESS_PORT", stringify(flags.submitter_run_port));
                process::initialize("submitter");

                Submitter submitter;
                submitter.setM_spark_path(flags.spark_path);

                string master_ip_and_port = "master@" + stringify(flags.master_ip_and_port);
                LOG(INFO) << master_ip_and_port;
                submitter.setDEFAULT_MASTER(master_ip_and_port);

                PID<Submitter> cur_submitter = process::spawn(submitter);
                LOG(INFO) << "Running submitter on " << process::address().ip << ":" << process::address().port;

                const PID<Submitter> submitter_pid = submitter.self();
                LOG(INFO) << submitter_pid;
                //    process::terminate(submitter.self());
                process::wait(submitter.self());
            } else {
                LOG(INFO) << "Enter all parameters: " << flags.usage();
            }

        }
    }
    return 0;
}

