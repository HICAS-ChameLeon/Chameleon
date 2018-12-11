/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */

#include "slave.hpp"
#include <slave_flags.hpp>

namespace chameleon {

    void Slave::initialize() {
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        msp_masterUPID = make_shared<UPID>(UPID(DEFAULT_MASTER));
        install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);
        install<JobMessage>(&Slave::get_a_job);


        HardwareResourcesMessage *hr_message = msp_resource_collector->collect_hardware_resources();
        DLOG(INFO) << *msp_masterUPID;
        string slave_id = stringify(self().address.ip);
        hr_message->set_slave_id(slave_id);
        DLOG(INFO) << "before send message to master";

        send(*msp_masterUPID, *hr_message);
        delete hr_message;
        LOG(INFO) << "The initialization if slave itself finished.";
        LOG(INFO) << self() << " starts to send heartbeat message to the master";
        heartbeat();
    }


    void Slave::register_feedback(const string &hostname) {
        cout << " receive register feedback from master" << hostname << endl;
    }

    void Slave::get_a_job(const UPID &master, const JobMessage &job_message) {
        LOG(INFO) << "slave " << self() << " got a job";
        const string test_spark_file = path::join(os::getcwd(), "lele_spark-2.3.0.tar");
//    ASSERT_SOME(os::write(test_spark_file,job_message.exe_file()));
        Try<string> decompressed_spark = gzip::decompress(job_message.exe_file());
        if (decompressed_spark.isError()) {
            LOG(ERROR) << "slave got a job file which is not completed or decompressing it had mistakes.";
            LOG(ERROR) << decompressed_spark.error();
        } else {
            ASSERT_SOME(os::write(test_spark_file, decompressed_spark.get()));
            LOG(INFO) << "slave " << self() << "successfully ungziped a job file";
        }
        const string shell_command = "tar xvf " + test_spark_file;
        string out = path::join(os::getcwd(), "stdout");
        string err = path::join(os::getcwd(), "stderr");

        Try<Subprocess> s = subprocess(
                shell_command,
                Subprocess::FD(STDIN_FILENO),
                Subprocess::PATH(out),
                Subprocess::PATH(err)
        );
        if (s.isError()) {
            LOG(ERROR) << "slave " << self() << "failed to untar the job file.";
            LOG(ERROR) << s.error();
        } else {
            LOG(INFO) << "slave " << self() << "successfully untar a job file";
            LOG(INFO) << "job_message is_is_master = " << job_message.is_master();
            if (job_message.is_master()) {
                const string fork_spark_master = "./spark-2.3.0-bin-hadoop2.7/sbin/start-master.sh";
                Try<ProcessTree> res = os::Fork(None(), os::Exec(fork_spark_master))();
                if (res.isError()) {
                    LOG(ERROR) << "slave " << self() << " failed to fork a process to run spark 2.3.0 master process";
                    LOG(ERROR) << res.error();
                } else {
                    LOG(INFO) << "slave " << self() << "successfully fork a process to run spark 2.3.0 master process";
                    LOG(INFO) << "The pid is " << res.get().children.front().process.pid;
                }
            } else {
                sleep(2);
                const string master_ip = job_message.master_ip();
                const string fork_spark_slave =
                        "./spark-2.3.0-bin-hadoop2.7/sbin/start-slave.sh spark://" + master_ip + ":7077";
                Try<ProcessTree> res = os::Fork(None(), os::Exec(fork_spark_slave))();
                if (res.isError()) {
                    LOG(ERROR) << "slave " << self() << " failed to fork a process to run spark 2.3.0 slave process";
                    LOG(ERROR) << res.error();
                } else {
                    LOG(INFO) << "slave " << self() << "successfully fork a process to run spark 2.3.0 slave process";
                    LOG(INFO) << "The pid is " << res.get().children.front().process.pid;
                }
            }


        }
    }

    void Slave::finalize() {
        ProcessBase::finalize();
        LOG(INFO) << "slave finalize()";
    }

    void Slave::heartbeat() {
        send_heartbeat_to_master();
        process::delay(m_interval, self(), &Self::heartbeat);

    }

    void Slave::send_heartbeat_to_master() {

        auto t1 = std::chrono::system_clock::now();

        RuntimeResourcesMessage *rr_message = new RuntimeResourcesMessage();
        RuntimeResourceUsage::CpuOccupy f_cpu, s_cpu;

        // get cpu usage
        msp_runtime_resource_usage->get_cpu_used_info(&f_cpu);
        usleep(100000); // we used 10^5 microseconds as the default duration for cpu usage calculation
        msp_runtime_resource_usage->get_cpu_used_info(&s_cpu);
        CPUUsage *cpu_usage = msp_runtime_resource_usage->cal_cpu_usage(&f_cpu, &s_cpu);
        rr_message->set_allocated_cpu_usage(cpu_usage);

        // get memory usage
        MemoryUsage *memory_usage = msp_runtime_resource_usage->select_memusage();
        rr_message->set_allocated_mem_usage(memory_usage);

        // get disk usage
        DiskUsage *disk_usage = msp_runtime_resource_usage->get_disk_usage();
        rr_message->set_allocated_disk_usage(disk_usage);

        // get current network speed
        NetUsage *net_usage = msp_runtime_resource_usage->cal_net_usage();
        rr_message->set_allocated_net_usage(net_usage);

        rr_message->set_slave_id(stringify(self().address.ip));

        send(*msp_masterUPID, *rr_message);
        LOG(INFO) << "slave " << self() << " had sent a heartbeat message to the " << *msp_masterUPID;
        auto t2 = std::chrono::system_clock::now();
        std::chrono::duration<double> duration = t2 - t1;
        LOG(INFO) << "It cost " << duration.count() << " s";
        delete rr_message;
    }

}

using namespace chameleon;

int main(int argc, char **argv) {
    chameleon::set_storage_paths_of_glog("slave");// provides the program name
    chameleon::set_flags_of_glog();

    LOG(INFO) << "glog files paths configuration for slave finished. OK!";


    /* Desciption : SlaveFlagsTest
     * Author     : weiguow
     * */
//    chameleon::SlaveFlagsBase slaveFlagsBase;
//    os::setenv("SLAVEFLAGS_slave_port","6061a");
//    Try<Warnings> load = slaveFlagsBase.load("SLAVEFLAGS");
//    string slport = std::to_string(slaveFlagsBase.slave_port);

    os::setenv("LIBPROCESS_PORT", stringify(6061));
    process::initialize("slave");

    Slave slave;
    PID<Slave> cur_slave = process::spawn(slave);
    LOG(INFO) << "Running slave on " << process::address().ip << ":" << process::address().port;


    const PID<Slave> slave_pid = slave.self();
    LOG(INFO) << slave_pid;
    process::wait(slave.self());
    return 0;
}