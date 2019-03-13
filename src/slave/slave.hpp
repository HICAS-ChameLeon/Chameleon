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

            void run_task(const process::UPID &from,
                    const mesos::FrameworkInfo &frameworkInfo,
                    const mesos::FrameworkID &frameworkId,
                    const process::UPID &pid,
                    const mesos::TaskInfo &task);

            void run(const mesos::FrameworkInfo& frameworkInfo,
                    const mesos::ExecutorInfo& executorInfo,
                    const Option<mesos::TaskInfo> &task,
                    const Option<mesos::TaskGroupInfo> &taskGroup,
                    const UPID &pid);

            void _run(const mesos::FrameworkInfo &frameworkInfo,
                    const mesos::ExecutorInfo& executorInfo,
                    const Option<mesos::TaskInfo> &task,
                    const Option<mesos::TaskGroupInfo> &taskGroup);

            void __run(const mesos::FrameworkInfo& frameworkInfo,
                    const mesos::ExecutorInfo& executorInfo,
                    const Option<mesos::TaskInfo> &task,
                    const Option<mesos::TaskGroupInfo> &taskGroup);

            void ___run(const mesos::FrameworkInfo& frameworkInfo,
                        const mesos::ExecutorInfo& executorInfo,
                        const Option<mesos::TaskInfo> &task,
                        const Option<mesos::TaskGroupInfo> &taskGroup);

            void register_executor(const UPID &from,
                    const mesos::FrameworkID &frameworkId,
                    const mesos::ExecutorID &executorId);

            void status_update(mesos::internal::StatusUpdate update, const Option<UPID> &pid);

            void send_status_update_to_executor(const mesos::internal::StatusUpdate &update,
                    const Option<UPID> &pid);

            void forward_status_update_to_master(mesos::internal::StatusUpdate update);

            void status_update_acknowledgement(const UPID &from,const mesos::SlaveID &slaveId,
                    const mesos::FrameworkID &frameworkId,const mesos::TaskID &taskId,
                    const string &uuid);

            Framework *get_framework(const mesos::FrameworkID &frameworkId) const;

            void launch_executor(const mesos::FrameworkID &frameworkId,
                    const mesos::ExecutorID &executorId,
                    const Option<mesos::TaskInfo> &taskInfo);

            void remove_framework(Framework *framework);

            void shutdown_framework(const process::UPID &from,
                    const mesos::FrameworkID &frameworkId);

            mesos::ExecutorInfo get_executorinfo(const mesos::FrameworkInfo &frameworkInfo,
                    const mesos::TaskInfo &task) const;

            void shutdown_executor(const UPID &from,const mesos::FrameworkID &frameworkId,
                    const mesos::ExecutorID &executorId);

            void setM_master(const string &m_master) {Slave::m_master = m_master;}

            void setM_interval(const Duration &m_interval) {Slave::m_interval = m_interval;}

            void setM_work_dir(const string &m_work_dir) {Slave::m_work_dir = m_work_dir;}

        protected:
            void finalize() override;

        private:
            friend class Framework;
            friend class Executor;

            shared_ptr<ResourceCollector> msp_resource_collector;
            shared_ptr<RuntimeResourceUsage> msp_runtime_resource_usage;

            shared_ptr<UPID> msp_masterUPID;

            string m_uuid;
            string m_master;
            string m_work_dir;
            Duration m_interval;

            hashmap<string, Framework *> m_frameworks;

            map<string, string> m_enviornment;
            mesos::SlaveInfo m_slaveInfo;
            mesos::ExecutorInfo m_executorInfo;

            vector<mesos::TaskInfo> m_tasks;

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

        class Executor {
        public:
            enum State {
                REGISTERING,  // Executor is launched but not (re-)registered yet.
                RUNNING,      // Executor has (re-)registered.
                TERMINATING,  // Executor is being shutdown/killed.
                TERMINATED,   // Executor has terminated but there might be pending updates.
            } state;

            Executor(Slave *_slave,
                     const mesos::FrameworkID &_frameworkId,
                     const mesos::ExecutorInfo &_info,
                     const mesos::ContainerID &_containerId
            ) : slave(_slave),
                frameworkId(_frameworkId),
                info(_info),
                containerId(_containerId){
            };

            ~Executor();

            friend std::ostream &operator<<(std::ostream &stream, const Executor &executor);

            template<typename Message>
            void send(const Message &message) {

                if (pid.isSome()) {
                    slave->send(pid.get(), message);
                } else {
                    LOG(WARNING) << "Unable to send event to executor " << *this
                                 << ": unknown connection type";
                }
            }

            //non-static data member
            Slave *slave;
            const mesos::ExecutorInfo info;
            Option<process::UPID> pid;
            const mesos::FrameworkID frameworkId;
            const mesos::ContainerID containerId;

            LinkedHashMap<string, mesos::TaskInfo> queuedTasks;
            std::list<mesos::TaskGroupInfo> queuedTaskGroups;

        private:
            Executor(const Executor &);              // No copying.
            Executor &operator=(const Executor &);   // No assigning.
        };

        class Framework {
        public:
            Framework(
                    Slave* _slave,
                    const mesos::FrameworkInfo &_info,
                    const Option<process::UPID> &_pid)
                    : slave(_slave),
                      info(_info) {
                pid = _pid;
            };

            ~Framework();

            const mesos::FrameworkID id() const { return info.id(); }

            Executor *add_executor(const mesos::ExecutorInfo &executorInfo);
            Executor *get_executor(const mesos::ExecutorID &executorId);

            enum State {
                RUNNING,      // First state of a newly created framework.
                TERMINATING,  // Framework is shutting down in the cluster.
            } state;

            Slave *slave;
            mesos::FrameworkInfo info;
            Option<process::UPID> pid;

            hashmap<string, Executor*> executors;

        private:
            Framework(const Framework &);
            Framework &operator=(const Framework &);
        };

        map<string, string> executor_environment(
                const mesos::ExecutorInfo &executorInfo,
                const mesos::SlaveID &slaveId,
                const process::PID<Slave> &slavePid
                );

        string taskOrTaskGroup(
                const Option<mesos::TaskInfo> &task,
                const Option<mesos::TaskGroupInfo> &taskGroup) {
            std::ostringstream out;
            if (task.isSome()) {
                out << "task '" << task->task_id().value() << "'";
            } else {
                CHECK_SOME(taskGroup);

                vector<mesos::TaskID> taskIds;
                foreach (const mesos::TaskInfo& task, taskGroup->tasks()) {
                    taskIds.push_back(task.task_id());
                }
                out << "task group containing tasks " << taskIds.size();
            }
            return out.str();
        }

        inline std::ostream& operator<<(std::ostream& stream, const Executor& executor)
        {
            stream << "'" << executor.info.executor_id().value() << "' of framework " << executor.frameworkId.value();

            if (executor.pid.isSome() && executor.pid.get()) {
                stream << " at " << executor.pid.get();
            }
            return stream;
        }

        inline std::ostream& operator<<(std::ostream& stream, Framework::State state)
        {
            switch (state) {
                case Framework::RUNNING:     return stream << "RUNNING";
                case Framework::TERMINATING: return stream << "TERMINATING";
                default:                     return stream << "UNKNOWN";
            }
        }

        inline std::ostream& operator<<(std::ostream& stream, Executor::State state)
        {
            switch (state) {
                case Executor::REGISTERING: return stream << "REGISTERING";
                case Executor::RUNNING:     return stream << "RUNNING";
                case Executor::TERMINATING: return stream << "TERMINATING";
                case Executor::TERMINATED:  return stream << "TERMINATED";
                default:                    return stream << "UNKNOWN";
            }
        }

        inline std::ostream &operator<<(std::ostream &stream, const mesos::TaskState &state) {
            return stream << TaskState_Name(state);
        }
    }
}


#endif //CHAMELEON_SLAVE_HPP
