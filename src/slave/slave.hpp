/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */
#ifndef CHAMELEON_PARTICIPANT_HPP
#define CHAMELEON_PARTICIPANT_HPP

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
#include <monitor_info.pb.h>
#include <job.pb.h>
#include <runtime_resource.pb.h>
#include <cluster_operation.pb.h>
#include <slave_related.pb.h>

#include <scheduler.pb.h>
#include <messages.pb.h>
#include <mesos.pb.h>

// chameleon headers
#include <resource_collector.hpp>
#include <configuration_glog.hpp>
#include <runtime_resources_usage.hpp>
#include <chameleon_os.hpp>
#include <chameleon_string.hpp>

using std::string;
using std::queue;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;
using std::to_string;

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
    class SlaveHeartbeater;

    class Slave;

    class Slave : public ProtobufProcess<Slave> {
    public:
        explicit Slave() : ProcessBase("slave"), m_interval() {
            msp_resource_collector = make_shared<ResourceCollector>(ResourceCollector());
            msp_runtime_resource_usage = make_shared<RuntimeResourceUsage>(RuntimeResourceUsage());
//            msp_resource_collector = new ResourceCollector();
        }

        Slave(const Slave &slave) = default;

        virtual ~Slave() {
            LOG(INFO) << "~ Slave()";
        }


    protected:
        void finalize() override;

    public:
        virtual void initialize();

        void register_feedback(const string &hostname);

        void get_a_job(const UPID &master, const JobMessage &job_message);

        void send_heartbeat_to_master();

        void setM_master(const string &m_master) {
            Slave::m_master = m_master;
        }

        void setM_interval(const Duration &m_interval) {
            Slave::m_interval = m_interval;
        }

        void runTask(const process::UPID& from,
                const mesos::FrameworkInfo& frameworkInfo,
                const mesos::FrameworkID& frameworkId,
                const process::UPID& pid,
                const mesos::TaskInfo& task);

        mesos::ExecutorInfo getExecutorInfo(
                const mesos::FrameworkInfo &frameworkInfo,
                const mesos::TaskInfo &task) const;

        void statusUpdate(mesos::internal::StatusUpdate update, const Option<UPID>& pid);

        void forward(mesos::internal::StatusUpdate update);

        void statusUpdateAcknowledgement(
                const UPID& from,
                const mesos::SlaveID& slaveId,
                const mesos::FrameworkID& frameworkId,
                const mesos::TaskID& taskId,
                const string& uuid);

    private:
        shared_ptr<ResourceCollector> msp_resource_collector;
        shared_ptr<RuntimeResourceUsage> msp_runtime_resource_usage;
//        Option<process::Owned<SlaveHeartbeater>> heartbeater;

        shared_ptr<UPID> msp_masterUPID;
        Duration m_interval;
        string m_uuid;
        string m_master;  //master@127.0.0.1：1080

        mesos::FrameworkInfo m_frameworkInfo;
        mesos::SlaveInfo m_slaveInfo;
        mesos::FrameworkID  m_frameworkID;
        mesos::ExecutorInfo m_executorInfo;

        // used a queue to keep track of the tasks awaiting to run
//        mesos::TaskInfo m_task;
        queue<mesos::TaskInfo> m_tasks;
        mesos::SlaveID m_slaveID;

        void heartbeat();

        void shutdown(const UPID &master, const ShutdownMessage &shutdown_message);

        void start_mesos_executor();

        void registerExecutor(const UPID& from,
                              const mesos::FrameworkID& frameworkId,
                              const mesos::ExecutorID& executorId);

        void _statusUpdate(
                const mesos::internal::StatusUpdate& update,
                const Option<UPID>& pid);

        void reregister_to_master(const UPID& from, const ReregisterMasterMessage& message);

    };

    std::ostream& operator<<(std::ostream& stream, const mesos::TaskState& state);

    class SlaveHeartbeater : public process::Process<SlaveHeartbeater> {

    public:

        SlaveHeartbeater(const Duration &interval)
                : process::ProcessBase(process::ID::generate("slaveheartbeater")),
                  m_interval(interval) {
        }

        virtual void initialize() {
            heartbeat();
//        install<Offer>(&Master::report_from_client, &Offer::key,&Offer::value);
        }

        void setM_interval(Duration &m_interval) {
            SlaveHeartbeater::m_interval = m_interval;
        }

    private:

        void heartbeat() {
            DLOG(INFO) << "5 seconds";
            //  delays 5 seconds to invoke the function "heartbeat " of self.
            // it's cyclical because "heartbeat invoke heartbeat"
            process::delay(m_interval, self(), &Self::heartbeat);
        }
        Duration m_interval;
    };

}


#endif //CHAMELEON_PARTICIPANT_HPP
