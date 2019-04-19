/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-4-19
 * Description：risc-v slave codes
 */

#ifndef CHAMELEON_RISCV_SLAVE_HPP
#define CHAMELEON_RISCV_SLAVE_HPP

// C++ 11 dependencies
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <queue>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>
#include <stout/path.hpp>
#include <stout/uuid.hpp>

#include <stout/flags.hpp>
#include <stout/hashmap.hpp>
#include <stout/hashset.hpp>
#include <stout/lambda.hpp>
#include <stout/option.hpp>
#include <stout/try.hpp>

#include <stout/os/getcwd.hpp>
#include <stout/os/write.hpp>

#include <stout/tests/utils.hpp>
#include <stout/gzip.hpp>
#include <stout/boundedhashmap.hpp>
#include <stout/linkedhashmap.hpp>

// boost dependencies
#include <boost/circular_buffer.hpp>


// libprocess dependencies
#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/delay.hpp>
#include <process/subprocess.hpp>

// protobuf
//#include <monitor_info.pb.h>
#include <runtime_resource.pb.h>


// chameleon headers

#include <configuration_glog.hpp>
#include <chameleon_os.hpp>
#include <chameleon_string.hpp>


using std::string;
using std::queue;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::map;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;


namespace chameleon {
    // forward declations
//    class SlaveHeartbeater;
//
//    class Executor;
//
//    class Framework;

    class RiscvSlave : public ProtobufProcess<RiscvSlave> {
    public:
        explicit RiscvSlave();

        RiscvSlave(const RiscvSlave &slave) = delete;

        virtual ~RiscvSlave();

        virtual void initialize();

        void register_feedback(const string &hostname);

        void heartbeat();

        void send_heartbeat_to_master();

        void setM_master(const string &m_master) {
            RiscvSlave::m_master = m_master;
        }

        void setM_interval(const Duration &m_interval) {
            RiscvSlave::m_interval = m_interval;
        }

        void setM_work_dir(const string &m_work_dir) {
            RiscvSlave::m_work_dir = m_work_dir;
        }

        // fault tolerance related
//        void setM_fault_tolerance(bool m_fault_tolerance){
//            Slave::m_is_fault_tolerance = m_fault_tolerance;
//        }

//        void run_task(const process::UPID &from,
//                      const mesos::FrameworkInfo &frameworkInfo,
//                      const mesos::FrameworkID &frameworkId,
//                      const process::UPID &pid,
//                      const mesos::TaskInfo &task);
//
//        void status_update(mesos::internal::StatusUpdate update, const Option<UPID> &pid);
//
//        void status_update_acknowledgement(
//                const UPID &from,
//                const mesos::SlaveID &slaveId,
//                const mesos::FrameworkID &frameworkId,
//                const mesos::TaskID &taskId,
//                const string &uuid);
//
//        Framework *get_framework(
//                const mesos::FrameworkID &frameworkId) const;
//
//        mesos::ExecutorInfo get_executorinfo(
//                const mesos::FrameworkInfo &frameworkInfo,
//                const mesos::TaskInfo &task) const;
//
//        void remove_framework(Framework* framework);
//
//        void shutdown_framework(
//                const process::UPID& from,
//                const mesos::FrameworkID& frameworkId);

//    protected:
//        void finalize() override;

    private:
//        friend class Framework;
//
//        friend class Executor;
//
//        shared_ptr<ResourceCollector> msp_resource_collector;
//        shared_ptr<RuntimeResourceUsage> msp_runtime_resource_usage;
//        RuntimeResourcesMessage m_runtime_resources;
////        Option<process::Owned<SlaveHeartbeater>> heartbeater;
//        HardwareResourcesMessage *hr_message;

        shared_ptr<UPID> msp_masterUPID;
        Duration m_interval;
        string m_uuid;
        string m_master;  //master@127.0.0.1：1080
        UPID m_backup_master;

//        hashmap<string, Framework *> frameworks;
//
//        mesos::SlaveInfo m_slaveInfo;
//        mesos::ExecutorInfo m_executorInfo;


        string m_work_dir;
        // the absolute path of the slave executable
        string m_cwd;

        // fault tolerance related
        bool m_is_fault_tolerance = false;

//        BoundedHashMap<mesos::FrameworkID, process::Owned<Framework>> completedFrameworks;

        // used a queue to keep track of the tasks awaiting to run
//        mesos::TaskInfo m_task;
//        queue<mesos::TaskInfo> m_tasks;
//        mesos::SlaveID m_slaveID;

        // software resources related
//        SoftwareResourceManager* m_software_resource_manager;
//
//        void heartbeat();
//
//        void shutdown(const UPID &master, const ShutdownMessage &shutdown_message);
//
//        void start_mesos_executor(const Future<Nothing>& future, const Framework *framework);
//
//        void start_docker_container(const mesos::TaskInfo& taskInfo, const Framework *framework);
//
//        void register_executor(const UPID &from,
//                               const mesos::FrameworkID &frameworkId,
//                               const mesos::ExecutorID &executorId);
//
//        void reregister_to_master(const UPID &from, const ReregisterMasterMessage &message);
//
//        void launch_master(const UPID &super_master, const LaunchMasterMessage &message);
//
//        // fault tolerance related
//        void send_message_to_backup_master(const UPID &master, const BackupMasterMessage &message);
//
//        //super_master related
////        void received_new_master(const UPID& from, const MasterRegisteredMessage& message);
//
//        // running task related
//        void modify_command_info_of_running_task(const string& spark_home_path, mesos::TaskInfo &task);
//
//        void modify_command_info_of_flink_task(const string& spark_home_path, mesos::TaskInfo &task);
    };

//    class Framework {
//    public:
//        Framework(
//                RiscvSlave *_slave,
//                const mesos::FrameworkInfo &_info,
//                const Option<process::UPID> &_pid)
//                : slave(_slave),
//                  info(_info) {
//            pid = _pid;
//        };
//
//        ~Framework();
//
//        const mesos::FrameworkID id() const { return info.id(); }
//
//        enum State {
//            RUNNING,      // First state of a newly created framework.
//            TERMINATING,  // Framework is shutting down in the cluster.
//        } state;
//
//        RiscvSlave *slave;
//        mesos::FrameworkInfo info;
//        Option<process::UPID> pid;
//
//    private:
//        Framework(const Framework &);
//
//        Framework &operator=(const Framework &);
//    };
//
////    inline std::ostream &operator<<(std::ostream &stream, const mesos::TaskState &state) {
////        return stream << TaskState_Name(state);
////    };
//
//    std::ostream &operator<<(std::ostream &stream, Framework::State state);

}


#endif //CHAMELEON_RISCV_SLAVE_HPP
