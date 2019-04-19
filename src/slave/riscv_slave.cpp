/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-4-19
 * Description：risc-v RiscvSlave codes
 */

#include "riscv_slave.hpp"


//The following flags has default values
DEFINE_uint32(ht, 6, "Heartbeat interval");
DEFINE_int32(port, 6061, "port");

//The following flags must be set by user
DEFINE_string(master, "", "master ip and port info");
DEFINE_string(work_dir, "work_dir",
              "the path to store the files of frameworks. The default is build/src/RiscvSlave/work_dir");
//DEFINE_bool(fault_tolerance, false,"whether master has fault tolerance. For example, --fault_tolerance=true");

/**
 * Function name  : ValidateStr
 * Author         : weiguow
 * Date           : 2018-12-13
 * Description    : Determines whether the input parameter is valid
 * Return         : True or False*/
static bool ValidateStr(const char *flagname, const string &value) {
    if (!value.empty()) {
        return true;
    }
    printf("Invalid value for --%s: To run this program, you must set a meaningful value for it "
           "%s\n", flagname, value.c_str());;
    return false;
}

static bool ValidateInt(const char *flagname, gflags::int32 value) {
    if (value >= 0 && value < 32768) {
        return true;
    }
    printf("Invalid value for --%s %d\n", flagname, (int) value);
    return false;
}

static bool ValidateUint(const char *flagname, gflags::uint32 value) {
    if (value >= 2) {
        return true;
    }
    printf("Invalid value for --%s %d\n", flagname, (int) value);
    return false;
}

static bool validate_work_dir(const char *flagname, const string &value) {

    if (value == "work_dir" || os::exists(value)) {
        return true;
    }
    printf("Invalid value for work_dir, please make sure the work_dir actually exist!");
    return false;

}

static const bool ht_Uint = gflags::RegisterFlagValidator(&FLAGS_ht, &ValidateUint);
static const bool port_Int = gflags::RegisterFlagValidator(&FLAGS_port, &ValidateInt);
static const bool master_Str = gflags::RegisterFlagValidator(&FLAGS_master, &ValidateStr);
static const bool work_dir_Str = gflags::RegisterFlagValidator(&FLAGS_work_dir, &validate_work_dir);

constexpr char MESOS_EXECUTOR[] = "chameleon-executor";

namespace chameleon {

    RiscvSlave::RiscvSlave() : ProcessBase("RiscvSlave"), m_interval() {


    }

    RiscvSlave::~RiscvSlave() {

    }


    void RiscvSlave::initialize() {
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        m_uuid = UUID::random().toString();

//        LOG(INFO) << "RiscvSlave executable path" << setting::RiscvSlave_EXE_DIR;

        msp_masterUPID = make_shared<UPID>(UPID(m_master));

//        m_RiscvSlaveInfo.set_hostname(self().address.hostname().get());
//        m_RiscvSlaveInfo.mutable_id()->set_value(m_uuid);
//        m_RiscvSlaveID.set_value(m_uuid);
//        m_RiscvSlaveInfo.set_port(self().address.port);


//        send(*msp_masterUPID, *hr_message);

        LOG(INFO) << "The initialization of RiscvSlave itself finished.";
        LOG(INFO) << self() << " starts to send heartbeat message to the master";

    }

    void RiscvSlave::heartbeat(){
        send_heartbeat_to_master();
        process::delay(m_interval, self(), &Self::heartbeat);
    }
    void RiscvSlave::send_heartbeat_to_master() {
        RiscvHeartbeatMessage* heartbeat = new RiscvHeartbeatMessage();
        heartbeat->set_slave_ip(stringify(self().address.ip));
        heartbeat->set_slave_uuid(m_uuid);
        heartbeat->set_slave_port(stringify(self().address.port));
        heartbeat->set_host_desc("Linux (lvna) 4.18.0-ga57318a4 #7 SMP Thu Jan 24 14:42:58 CST 2019 riscv64 GNU/Linux");
        send(*msp_masterUPID, *heartbeat);
        delete heartbeat;

    }
}


using namespace chameleon;

int main(int argc, char *argv[]) {
    chameleon::set_storage_paths_of_glog("RiscvSlave");// provides the program name
    chameleon::set_flags_of_glog();


    google::SetUsageMessage("usage : Option[name] \n"
                            "--port      the port used by the program \n"
                            "--master    the master ip and port,example:127.0.0.1:8080 \n"
                            "--ht        fixed time interval, RiscvSlave send message to master \n"
                            "            and the interval >= 2 \n"
                            "--work_dir  the path to store download file");
    google::SetVersionString("Chameleon v1.0");
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::CommandLineFlagInfo info;


    if (master_Str && port_Int && work_dir_Str) {
        // first get the absolute path for the default work_dir
        string work_dir_path = FLAGS_work_dir;
        if (FLAGS_work_dir == "work_dir") {
            work_dir_path = path::join(os::getcwd(), FLAGS_work_dir);
            Try<Nothing> work_dir_create = os::mkdir(work_dir_path);
            if (work_dir_create.isError()) {
                printf(work_dir_create.error().c_str());
                return -1;
            }
        }

        os::setenv("LIBPROCESS_IP", "192.168.1.2");
//        os::setenv("LIBPROCESS_PORT", "6061");
//
////        os::setenv("LIBPROCESS_PORT", stringify(FLAGS_port));  // LIBPROCESS_
//        LOG(INFO)<<"Heldon env port : "<< os::getenv("LIBPROCESS_PORT").get();
        process::initialize("RiscvSlave");

        chameleon::RiscvSlave riscv_slave;

        riscv_slave.setM_interval(Seconds(FLAGS_ht));
        riscv_slave.setM_work_dir(work_dir_path);
//        RiscvSlave.setM_fault_tolerance(FLAGS_fault_tolerance);

        string master_ip_and_port = "master@" + stringify(FLAGS_master);
        riscv_slave.setM_master(master_ip_and_port);
        PID<chameleon::RiscvSlave> cur_riscv_slave = process::spawn(riscv_slave);
        LOG(INFO) << "Running RiscvSlave on " << process::address().ip << ":" << process::address().port;
        const PID<chameleon::RiscvSlave> riscv_slave_pid = riscv_slave.self();
        LOG(INFO) << riscv_slave_pid;
        process::wait(riscv_slave.self());

    } else {
        LOG(INFO) << "To run this program , must set all parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    }

    return 0;
}


