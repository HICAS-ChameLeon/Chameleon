/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */
#ifndef CHAMELEON_SLAVE_HPP
#define CHAMELEON_SLAVE_HPP

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
#include <monitor_info.pb.h>
#include <job.pb.h>
#include <runtime_resource.pb.h>
#include <cluster_operation.pb.h>
#include <slave_related.pb.h>
#include <scheduler.pb.h>
#include <messages.pb.h>
#include <mesos.pb.h>

// chameleon headers
#include <resource_collector/resource_collector.hpp>
#include <resource_collector/runtime_resources_usage.hpp>
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
    namespace slave {

        // forward declations
        class SlaveHeartbeater;
        class Executor;
        class Framework;

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

            virtual void initialize();

            void register_feedback(const string &hostname);

            void send_heartbeat_to_master();

            void setM_master(const string &m_master) {
                Slave::m_master = m_master;
            }

            void setM_interval(const Duration &m_interval) {
                Slave::m_interval = m_interval;
            }

            void setM_work_dir(const string &m_work_dir) {
                Slave::m_work_dir = m_work_dir;
            }

            void run_task(const process::UPID &from,
                          const mesos::FrameworkInfo &frameworkInfo,
                          const mesos::FrameworkID &frameworkId,
                          const process::UPID &pid,
                          const mesos::TaskInfo &task);

            void start_mesos_executor(const Framework *framework);

            void register_executor(const UPID &from,
                                   const mesos::FrameworkID &frameworkId,
                                   const mesos::ExecutorID &executorId);

            void status_update(mesos::internal::StatusUpdate update, const Option<UPID> &pid);

            void _status_update(const mesos::internal::StatusUpdate &update,
                                const Option<UPID> &pid);

            void forward_status_update(mesos::internal::StatusUpdate update);

            void status_update_acknowledgement(
                    const UPID &from,
                    const mesos::SlaveID &slaveId,
                    const mesos::FrameworkID &frameworkId,
                    const mesos::TaskID &taskId,
                    const string &uuid);

            Framework *get_framework(
                    const mesos::FrameworkID &frameworkId) const;

            void remove_framework(Framework *framework);

            void shutdown_framework(
                    const process::UPID &from,
                    const mesos::FrameworkID &frameworkId);

            mesos::ExecutorInfo get_executorinfo(
                    const mesos::FrameworkInfo &frameworkInfo,
                    const mesos::TaskInfo &task) const;

        protected:
            void finalize() override;

        private:
            friend class Framework;

            friend class Executor;

            shared_ptr<ResourceCollector> msp_resource_collector;
            shared_ptr<RuntimeResourceUsage> msp_runtime_resource_usage;
//        Option<process::Owned<SlaveHeartbeater>> heartbeater;

            shared_ptr<UPID> msp_masterUPID;
            Duration m_interval;

            string m_uuid;
            string m_master;  //master@127.0.0.1：1080

            hashmap<string, Framework *> m_frameworks;

            mesos::SlaveInfo m_slaveInfo;
            mesos::ExecutorInfo m_executorInfo;

            queue<mesos::TaskInfo> m_tasks;

            string m_work_dir;

            void heartbeat();

            void shutdown(const UPID &master, const ShutdownMessage &shutdown_message);

            void reregister_to_master(const UPID &from, const ReregisterMasterMessage &message);

        };

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

//    class Executor {
//    public:
//        Executor(Slave *slave,
//                 const mesos::FrameworkID &frameworkId,
//                 const mesos::ExecutorInfo &info,
//                 const mesos::ContainerID &_containerId,
//                 const std::string &directory,
//                 const Option<std::string> &user,
//                 bool checkpoint);
//
//        ~Executor();
//
//        void checkpointExecutor();
//
//        void checkpointTask(const mesos::TaskInfo &task);
//
//        mesos::Task *Executor::addTask(const mesos::TaskInfo &task);
//
//        friend std::ostream &operator<<(
//                std::ostream &stream,
//                const Executor &executor);
//
//        bool isCommandExecutor() const;
//
//        template<typename Message>
//        void send(const Message &message) {
//            if (state == REGISTERING || state == TERMINATED) {
//                LOG(WARNING) << "Attempting to send message to disconnected"
//                             << " executor " << *this << " in state " << state;
//            }
//            if (pid.isSome()) {
//                slave->send(pid.get(), message);
//            } else {
//                LOG(WARNING) << "Unable to send event to executor " << *this
//                             << ": unknown connection type";
//            }
//        }
//
//        //non-static data member
//        Slave *slave;
//        const mesos::ExecutorID id;
//        const mesos::ExecutorInfo info;
//        const mesos::FrameworkID frameworkId;
//        const std::string directory;
//        const mesos::ContainerID containerId;
//        const Option<std::string> user;
//        const bool checkpoint;
//        Option<process::UPID> pid;
//
//        LinkedHashMap<mesos::TaskID, mesos::TaskInfo> queuedTasks;
//        std::list<mesos::TaskGroupInfo> queuedTaskGroups;
//        LinkedHashMap<mesos::TaskID, mesos::Task *> launchedTasks;
//        LinkedHashMap<mesos::TaskID, mesos::Task *> terminatedTasks;
//
//
//        enum State {
//            REGISTERING,  // Executor is launched but not (re-)registered yet.
//            RUNNING,      // Executor has (re-)registered.
//            TERMINATING,  // Executor is being shutdown/killed.
//            TERMINATED,   // Executor has terminated but there might be pending updates.
//        } state;
//
//
//    private:
//        Executor(const Executor &);              // No copying.
//        Executor &operator=(const Executor &);   // No assigning.
//        bool commandExecutor;
//    };

        class Framework {
        public:
            Framework(
                    Slave *_slave,
                    const mesos::FrameworkInfo &_info,
                    const Option<process::UPID> &_pid)
                    : m_slave(_slave),
                      m_info(_info) {
                m_pid = _pid;
            };

            ~Framework();

//        Executor *addExecutor(const mesos::ExecutorInfo &executorInfo);

//        Executor *getExecutor(const mesos::ExecutorID &executorId) const;

//        void checkpointFramework() const;

            const mesos::FrameworkID id() const { return m_info.id(); }

            enum State {
                RUNNING,      // First state of a newly created framework.
                TERMINATING,  // Framework is shutting down in the cluster.
            } state;

            Slave *m_slave;
            mesos::FrameworkInfo m_info;
            Option<process::UPID> m_pid;

        private:
            Framework(const Framework &);

            Framework &operator=(const Framework &);
        };

        mesos::Task createTask(
                const mesos::TaskInfo &task,
                const mesos::TaskState &state,
                const mesos::FrameworkID &frameworkId);

        std::map<std::string, std::string> executorEnvironment(
                const mesos::ExecutorInfo &executorInfo,
                const std::string &directory,
                const mesos::SlaveID &slaveId,
                const process::PID<Slave> &slavePid,
                bool checkpoint);


        string taskOrTaskGroup(
                const Option<mesos::TaskInfo> &task,
                const Option<mesos::TaskGroupInfo> &taskGroup);

        std::ostream &operator<<(std::ostream &stream, const mesos::TaskState &state) {
            return stream << TaskState_Name(state);
        }

        std::ostream &operator<<(std::ostream &stream, Framework::State state);
    }
}


#endif //CHAMELEON_SLAVE_HPP
