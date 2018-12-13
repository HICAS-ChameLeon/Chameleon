//
// Created by lemaker on 18-12-4.
//

#include "submitter.hpp"

DEFINE_string(minfo,"127.0.0.1:8080", "ip and port info");
DEFINE_int32(port,0, "port");
DEFINE_string(path,"","The path where the spark package exists");

/*
 * Function name  : ValidateStr
 * Author         : weiguow
 * Date           : 2018-12-13
 * Description    : Determines whether the input parameter is valid
 * Return         : True or False*/
static bool ValidateStr(const char *flagname, const string &value) {
    if (!value.empty()) {
        return true;
    }
    printf("Invalid value for --%s: %s\n", flagname, value.c_str());;
    return false;
}

static bool ValidateInt(const char *flagname, gflags::int32 value) {
    if (value >= 0 && value < 32768) {
        return true;
    }
    printf("Invalid value for --%s: %d\n", flagname, (int) value);
    return false;
}

static const bool port_dummyInt = gflags::RegisterFlagValidator(&FLAGS_port, &ValidateInt);
static const bool minfo_dummyStr = gflags::RegisterFlagValidator(&FLAGS_minfo, &ValidateStr);

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

    google::SetUsageMessage("usage : Option[name] \n"
                            "--port      the port used by the program \n"
                            "--minfo     the master ip and port,example:127.0.0.1:8080 \n"
                            "--path      the path where the spark package exists");
    google::SetVersionString("Chameleon v1.0");
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::CommandLineFlagInfo info;

    if (GetCommandLineFlagInfo("port", &info) && info.is_default &&
        GetCommandLineFlagInfo("minfo", &info) && info.is_default &&
        GetCommandLineFlagInfo("path", &info) && info.is_default) {
        LOG(INFO) << "To run this program , must set parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    } else {
        os::setenv("LIBPROCESS_PORT",stringify(FLAGS_port));
        process::initialize("submitter");

        Submitter submitter;
        submitter.setM_spark_path(FLAGS_path);

        string master_ip_and_port = "master@" + stringify(FLAGS_minfo);
        submitter.setDEFAULT_MASTER(master_ip_and_port);

        PID<Submitter> cur_submitter = process::spawn(submitter);
        LOG(INFO) << "Running submitter on " << process::address().ip << ":" << process::address().port;

        const PID<Submitter> submitter_pid = submitter.self();
        LOG(INFO) << submitter_pid;
        //    process::terminate(submitter.self());
        process::wait(submitter.self());
    }
    return 0;
}

