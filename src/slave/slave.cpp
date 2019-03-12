/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */
#include "slave.hpp"

//The following flags has default values
DEFINE_uint32(ht, 6, "Heartbeat interval");
DEFINE_int32(port, 6061, "port");

//The following flags must be set by user
DEFINE_string(master, "", "master ip and port info");
DEFINE_string(work_dir, "", "the path to store download file");

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

static const bool ht_Uint = gflags::RegisterFlagValidator(&FLAGS_ht, &ValidateUint);
static const bool port_Int = gflags::RegisterFlagValidator(&FLAGS_port, &ValidateInt);
static const bool master_Str = gflags::RegisterFlagValidator(&FLAGS_master, &ValidateStr);
//static const bool work_dir_Str = gflags::RegisterFlagValidator(&FLAGS_work_dir, &ValidateStr);

constexpr char MESOS_EXECUTOR[] = "chameleon-executor";

namespace chameleon {
    namespace slave {

        void Slave::initialize() {
            // Verify that the version of the library that we linked against is
            // compatible with the version of the headers we compiled against.
            GOOGLE_PROTOBUF_VERIFY_VERSION;

            msp_masterUPID = make_shared<UPID>(UPID(m_master));

            m_slaveInfo.set_hostname(self().address.hostname().get());
            m_slaveInfo.set_port(self().address.port);
            m_slaveInfo.mutable_id()->set_value(m_uuid);

            install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);
            install<ShutdownMessage>(&Slave::shutdown);

            //get from executor
            install<mesos::internal::StatusUpdateMessage>(
                    &Slave::status_update,
                    &mesos::internal::StatusUpdateMessage::update,
                    &mesos::internal::StatusUpdateMessage::pid);

            install<mesos::internal::StatusUpdateAcknowledgementMessage>(
                    &Slave::status_update_acknowledgement,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::slave_id,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::framework_id,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::task_id,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::uuid);


            install<mesos::internal::RunTaskMessage>(
                    &Slave::run_task,
                    &mesos::internal::RunTaskMessage::framework,
                    &mesos::internal::RunTaskMessage::framework_id,
                    &mesos::internal::RunTaskMessage::pid,
                    &mesos::internal::RunTaskMessage::task);

            install<mesos::internal::RegisterExecutorMessage>(
                    &Slave::register_executor,
                    &mesos::internal::RegisterExecutorMessage::framework_id,
                    &mesos::internal::RegisterExecutorMessage::executor_id);

            install<mesos::internal::ShutdownExecutorMessage>(
                    &Slave::shutdown_executor,
                    &mesos::internal::ShutdownExecutorMessage::framework_id,
                    &mesos::internal::ShutdownExecutorMessage::executor_id);

            install<mesos::internal::ShutdownFrameworkMessage>(
                    &Slave::shutdown_framework,
                    &mesos::internal::ShutdownFrameworkMessage::framework_id);

            install<ReregisterMasterMessage>(&Slave::reregister_to_master);


            HardwareResourcesMessage *hr_message = msp_resource_collector->collect_hardware_resources();

            m_uuid = UUID::random().toString();
            hr_message->set_slave_uuid(m_uuid);
            hr_message->set_slave_hostname(self().address.hostname().get());
            hr_message->set_slave_id(stringify(self().address.ip));
            send(*msp_masterUPID, *hr_message);

            delete hr_message;

            LOG(INFO) << self() << " starts to send heartbeat message to the master";

            heartbeat();
        }

        void Slave::run_task(
                const process::UPID &from,
                const mesos::FrameworkInfo &frameworkInfo,
                const mesos::FrameworkID &frameworkId,
                const process::UPID &pid,
                const mesos::TaskInfo &task) {
            if (UPID(m_master) != from) {
                LOG(WARNING) << "Ignoring run task message from " << from
                             << " because it is not the expected master";
                return;
            }

            if (!frameworkInfo.has_id()) {
                LOG(ERROR) << "Ignoring run task message from " << from
                           << " because it does not have a framework ID";
                return;
            }

            const mesos::ExecutorInfo executorInfo = get_executorinfo(frameworkInfo, task);

            //pid is scheduler-b3ff430b-b93e-4a1b-8716-a5a1d6416fe5@172.20.110.98:45457
            run(frameworkInfo, executorInfo, task, None(), pid);
        }

        void Slave::run(const mesos::FrameworkInfo &frameworkInfo, const mesos::ExecutorInfo &executorInfo,
                        const Option<mesos::TaskInfo> &task, const Option<mesos::TaskGroupInfo> &taskGroup,
                        const UPID &pid) {

            const mesos::FrameworkID &frameworkId = frameworkInfo.id();
            LOG(INFO) << "Got assigned " << taskOrTaskGroup(task, taskGroup)
                      << " for framework " << frameworkId.value();

            //New Framework in slave
            Framework* framework = get_framework(frameworkId);
            if (framework == nullptr) {
                Option<UPID> frameworkPid = None();

                if (pid != UPID()) {
                    frameworkPid = pid;
                }

                framework = new Framework(this, frameworkInfo, frameworkPid);
                m_frameworks[frameworkId.value()] = framework;
            }

            process::dispatch(self(), &Self::_run, frameworkInfo,
                    executorInfo, task, taskGroup);
        }

        void Slave::_run(const mesos::FrameworkInfo &frameworkInfo, const mesos::ExecutorInfo &executorInfo,
                         const Option<mesos::TaskInfo> &task, const Option<mesos::TaskGroupInfo> &taskGroup) {

            LOG(INFO) << "Authorizing " << taskOrTaskGroup(task, taskGroup)
                      << " for framework " << frameworkInfo.id().value();

            Framework* framework = get_framework(frameworkInfo.id());

            if (framework == nullptr) {
                LOG(WARNING) << "Ignoring running " << taskOrTaskGroup(task, taskGroup)
                             << " because the framework " << framework->id().value()
                             << " does not exist";
                return;
            }

            LOG(INFO) << "Launching " << taskOrTaskGroup(task, taskGroup)
                      << " for framework " << framework->id().value();

            Executor *executor = framework->get_executor(executorInfo.executor_id());

            if (executor == nullptr) {
                //add executor && launchexecutor
                executor = framework->add_executor(executorInfo);

                launch_executor(
                        frameworkInfo.id(),
                        executor->info.executor_id(),
                        taskGroup.isNone() ? task.get() : Option<mesos::TaskInfo>::none());

            }

            process::dispatch(self(), &Self::__run, frameworkInfo,
                    executor->info, task, taskGroup);

        }

        void Slave::__run(const mesos::FrameworkInfo &frameworkInfo, const mesos::ExecutorInfo &executorInfo,
                          const Option<mesos::TaskInfo> &task, const Option<mesos::TaskGroupInfo> &taskGroup) {

            vector<mesos::TaskInfo> tasks;
            if (task.isSome()) {
                tasks.push_back(task.get());
            } else {
                foreach (const mesos::TaskInfo &_task, taskGroup->tasks()) {
                    tasks.push_back(_task);
                }
            }

            Framework* framework = get_framework(frameworkInfo.id());
            Executor *executor = framework->get_executor(executorInfo.executor_id());

            switch (executor->state) {
                case Executor::TERMINATING:
                case Executor::TERMINATED: {
                    string executorState;

                    if (executor->state == Executor::TERMINATING) {
                        executorState = "terminating";
                    } else {
                        executorState = "terminated";
                    }

                    LOG(WARNING) << "Asked to run " << taskOrTaskGroup(task, taskGroup)
                                 << "' for framework " << framework->id().value()
                                 << " with executor '" << executor->info.executor_id().value()
                                 << "' which is " << executorState;
                    break;
                }
                case Executor::REGISTERING: {
                    foreach (const mesos::TaskInfo &_task, tasks) {
                        // Queue task if the executor has not yet registered.
                        executor->queuedTasks[_task.task_id().value()] = _task;
                    }

                    if (taskGroup.isSome()) {
                        executor->queuedTaskGroups.push_back(taskGroup.get());
                    }

                    LOG(INFO) << "Queued " << taskOrTaskGroup(task, taskGroup)
                              << " for executor " << *executor;

                    Try<Subprocess> child = subprocess(
                            path::join(os::getcwd(), "mesos_executor/mesos-executor"),
                            Subprocess::FD(STDIN_FILENO),
                            Subprocess::FD(STDOUT_FILENO),
                            Subprocess::FD(STDERR_FILENO),
                            m_enviornment
                            );
                    break;
                }

                case Executor::RUNNING: {
                    foreach (const mesos::TaskInfo& _task, tasks) {
                        executor->queuedTasks[_task.task_id().value()] = _task;
                    }

                    if (taskGroup.isSome()) {
                        executor->queuedTaskGroups.push_back(taskGroup.get());
                    }

                    LOG(INFO) << "Queued " << taskOrTaskGroup(task, taskGroup)
                              << " for executor " << *executor;

                    break;
                }

                default:
                    LOG(FATAL) << "Executor " << *executor << " is in unexpected state "
                               << executor->state;
                    break;
            }

            const Duration delay_duration = Seconds(1);
            process::delay(delay_duration, self(), &Self::___run, frameworkInfo,
                              executor->info, task, taskGroup);
        }

        void Slave::___run(const mesos::FrameworkInfo &frameworkInfo, const mesos::ExecutorInfo &executorInfo,
                           const Option<mesos::TaskInfo> &task, const Option<mesos::TaskGroupInfo> &taskGroup) {
            vector<mesos::TaskInfo> tasks;
            if (task.isSome()) {
                tasks.push_back(task.get());
            } else {
                foreach (const mesos::TaskInfo &_task, taskGroup->tasks()) {
                    tasks.push_back(_task);
                }
            }

            Framework* framework = get_framework(frameworkInfo.id());
            Executor* executor = framework->get_executor(executorInfo.executor_id());

            foreach(const mesos::TaskInfo& task, tasks) {
                if (!executor->queuedTasks.contains(task.task_id().value())) {
                    LOG(WARNING) << "Ignoring send queued task "<< task.task_id().value()
                                 << " to " << *executor;
                    continue;
                }

                executor->queuedTasks.erase(task.task_id().value());

                mesos::internal::RunTaskMessage message;
                message.mutable_framework()->MergeFrom(framework->info);
                message.mutable_task()->MergeFrom(task);
                message.set_pid(framework->pid.getOrElse(UPID()));

                executor->send(message);
            }

        }

        void Slave::register_executor(const UPID &from,
                                      const mesos::FrameworkID &frameworkId,
                                      const mesos::ExecutorID &executorId) {


            LOG(INFO) << "Got registration for executor '" << executorId.value()
                      << "' of framework " << frameworkId.value() << " from "
                      << stringify(from);

            Framework *framework = get_framework(frameworkId);
            Executor* executor = framework->get_executor(executorId);

            switch (executor->state) {
                case Executor::TERMINATING:
                case Executor::TERMINATED:
                case Executor::RUNNING:
                    LOG(WARNING) << "Shutting down executor " <<*executor
                                 << " because it is unexpected state " << executor->state;
                    break;
                case Executor::REGISTERING: {
                    executor->state = Executor::RUNNING;

                    executor->pid = from;
                    link(from);

                    mesos::internal::ExecutorRegisteredMessage message;

                    message.mutable_executor_info()->MergeFrom(executor->info);
                    message.mutable_framework_id()->MergeFrom(framework->id());
                    message.mutable_framework_info()->MergeFrom(framework->info);
                    message.mutable_slave_id()->MergeFrom(m_slaveInfo.id());
                    message.mutable_slave_info()->MergeFrom(m_slaveInfo);

                    executor->send(message);

                    break;
                }
                default:
                    LOG(FATAL) << "Executor " << *executor << " is in unexpected state "
                    << executor->state;
            }

        }

        void Slave::status_update(mesos::internal::StatusUpdate update, const Option<UPID> &pid) {

            LOG(INFO) << "Handling status update " << update.status().state()
                      << " of framework " << update.framework_id().value();

            update.mutable_status()->set_uuid(update.uuid());
            update.mutable_status()->set_source(
                    pid == UPID() ? mesos::TaskStatus::SOURCE_SLAVE : mesos::TaskStatus::SOURCE_EXECUTOR);
            update.mutable_status()->mutable_executor_id()->CopyFrom(update.executor_id());

            LOG(INFO) << "Received status update " << update.status().state()
                      << " of framework " << update.framework_id().value();

            process::dispatch(self(), &Slave::forward_status_update_to_master, update);
        }

        void Slave::forward_status_update_to_master(mesos::internal::StatusUpdate update) {

            LOG(INFO) << "Forwarding the update " << update.status().state()
                      << " of framework " << update.framework_id().value() << " to " << m_master;

            Framework* framework = get_framework(update.framework_id());
            Executor* executor = framework->get_executor(update.executor_id());

            mesos::internal::StatusUpdateMessage message;
            message.mutable_update()->MergeFrom(update);
            message.set_pid(self()); // The ACK will be first received by the slave.

            send(m_master, message);

            process::dispatch(self(), &Slave::send_status_update_to_executor, update, executor->pid);
        }

        void Slave::send_status_update_to_executor(
                const mesos::internal::StatusUpdate &update,
                const Option<UPID> &pid) {

            mesos::internal::StatusUpdateAcknowledgementMessage message;
            message.mutable_framework_id()->MergeFrom(update.framework_id());
            message.mutable_slave_id()->MergeFrom(update.slave_id());
            message.mutable_task_id()->MergeFrom(update.status().task_id());
            message.set_uuid(update.uuid());

            if (pid.isSome()) {
                LOG(INFO) << "Sending acknowledgement for status update " << update.status().state()
                          << " of framework " << update.framework_id().value()
                          << " to " << pid.get();  //executor(1)@172.20.110.77:39343
                send(pid.get(), message);
            } else {
                LOG(INFO) << "Ignoring update status ";
            }
        }

        void Slave::status_update_acknowledgement(
                const UPID &from,
                const mesos::SlaveID &slaveId,
                const mesos::FrameworkID &frameworkId,
                const mesos::TaskID &taskId,
                const string &uuid) {

            if (UPID(m_master) != from) {
                LOG(WARNING) << "Ignoring status update acknowledgement message from "
                             << from << " because it is not the expected master: "
                             << m_master << " is None";
                return;
            }

            LOG(INFO) << "Status update manager successfully handled status update"
                      << " acknowledgement for task " << taskId.value()
                      << " of framework " << frameworkId.value();
        }

        Framework *Slave::get_framework(const mesos::FrameworkID &frameworkId) const {
            if (m_frameworks.count(frameworkId.value()) > 0) {
                return m_frameworks.at(frameworkId.value());
            }
            return nullptr;
        }

        Executor* Framework::get_executor(const mesos::ExecutorID &executorId) {
            if (executors.contains(executorId.value())) {
                return  executors.at(executorId.value());
            }
            return nullptr;
        }

        Executor* Framework::add_executor(const mesos::ExecutorInfo &executorInfo) {
            mesos::ContainerID containerId;
            containerId.set_value(UUID::random().toString());

            Executor* executor = new Executor(
                    slave,
                    id(),
                    executorInfo,
                    containerId
                    );

            executor->state = Executor::State::REGISTERING;

            executors[executorInfo.executor_id().value()] = executor;

            LOG(INFO) << "Launching executor '" << executorInfo.executor_id().value()
                      << "' of framework " << id().value() ;

            return executor;
        }

        void Slave::launch_executor(const mesos::FrameworkID &frameworkId, const mesos::ExecutorID &executorId,
                                    const Option<mesos::TaskInfo> &taskInfo) {
            Framework* framework = get_framework(frameworkId);
            Executor* executor = framework->get_executor(executorId);

            m_enviornment = executor_environment(
                    executor->info,
                    m_slaveInfo.id(),
                    self()
            );

            return;
            //launch container should be done in there
        }

        map<string, string> executor_environment(
                const mesos::ExecutorInfo &executorInfo,
                const mesos::SlaveID& slaveId,
                const PID<Slave> &slavepid
        ) {

            const string mesos_directory = path::join(os::getcwd(), "/mesos_executor/mesos-directory");

            map<string, string> environment;

            Option<string> libprocessIP = os::getenv("LIBPROCESS_IP");

            environment["LIBPROCESS_PORT"] = "0";
            environment["MESOS_FRAMEWORK_ID"] = executorInfo.framework_id().value();
            environment["MESOS_EXECUTOR_ID"] = executorInfo.executor_id().value();
            environment["MESOS_SLAVE_ID"] = slaveId.value();
            environment["MESOS_SLAVE_PID"] = stringify(slavepid);
            environment["MESOS_AGENT_ENDPOINT"] = stringify(slavepid.address);
            environment["MESOS_DIRECTORY"] = mesos_directory;
            environment["MESOS_CHECKPOINT"] = "0";

            return environment;
        }

        mesos::ExecutorInfo Slave::get_executorinfo(
                const mesos::FrameworkInfo &frameworkInfo,
                const mesos::TaskInfo &task) const {

            if (task.has_executor()) {
                return task.executor();
            }

            mesos::ExecutorInfo executorInfo;

            executorInfo.mutable_executor_id()->set_value(task.task_id().value());

            LOG(INFO) << "Generate new executorInfo, its executor_id is "
                      << executorInfo.mutable_executor_id()->value();

            executorInfo.mutable_framework_id()->CopyFrom(frameworkInfo.id());

            if (task.has_container()) {
                executorInfo.mutable_container()->CopyFrom(task.container());
            }

            string name = "(Task: " + task.task_id().value() + ") ";

            if (task.command().shell()) {
                if (!task.command().has_value()) {
                    name += "(Command: NO COMMAND)";
                } else {
                    name += "(Command: sh -c '";
                    if (task.command().value().length() > 15) {
                        name += task.command().value().substr(0, 12) + "...')";
                    } else {
                        name += task.command().value() + "')";
                    }
                }
            } else {
                if (!task.command().has_value()) {
                    name += "(Command: NO EXECUTABLE)";
                } else {
                    string args =
                            task.command().value() + ", " +
                            strings::join(", ", task.command().arguments());

                    if (args.length() > 15) {
                        name += "(Command: [" + args.substr(0, 12) + "...])";
                    } else {
                        name += "(Command: [" + args + "])";
                    }
                }
            }

            executorInfo.set_name("Command Executor " + name);
            executorInfo.set_source(task.task_id().value());

            executorInfo.mutable_command()->mutable_uris()->MergeFrom(
                    task.command().uris());

            if (task.command().has_environment()) {
                executorInfo.mutable_command()->mutable_environment()->MergeFrom(
                        task.command().environment());
            }

            // Add fields which can be relevant (depending on Authorizer) for
            // authorization.
            if (task.has_labels()) {
                executorInfo.mutable_labels()->MergeFrom(task.labels());
            }

            if (task.has_discovery()) {
                executorInfo.mutable_discovery()->MergeFrom(task.discovery());
            }

            if (task.command().has_user()) {
                executorInfo.mutable_command()->set_user(task.command().user());
            }

            Result<string> path = os::realpath(
                    path::join(setting::FLAGS_LAUCHER_DIR, MESOS_EXECUTOR));

            if (path.isSome()) {
                executorInfo.mutable_command()->set_shell(false);
                executorInfo.mutable_command()->set_value(path.get());
                executorInfo.mutable_command()->add_arguments(MESOS_EXECUTOR);
                executorInfo.mutable_command()->add_arguments(
                        "--launcher_dir=" + setting::FLAGS_LAUCHER_DIR);
            } else {
                executorInfo.mutable_command()->set_shell(true);
                executorInfo.mutable_command()->set_value(
                        "echo '" +
                        (path.isError() ? path.error() : "No such file or directory") +
                        "'; exit 1");
            }
            return executorInfo;
        }

        void Slave::shutdown_framework(const process::UPID &from, const mesos::FrameworkID &frameworkId) {
            LOG(INFO) << "Asked to shutdown framework " << frameworkId.value()
                      << " by " << from;

            Framework *framework = get_framework(frameworkId);

            switch (framework->state) {
                case Framework::TERMINATING:
                    LOG(WARNING) << "Ignoring shutdown framework " << framework->id().value()
                                 << " because it is terminating";
                    break;

                case Framework::RUNNING:
                    LOG(INFO) << "Shutting down framework " << framework->id().value();
                    framework->state = Framework::TERMINATING;
                    // Remove this framework if it has no pending executors and tasks.
                    remove_framework(framework);
                    break;

                default:
                    break;
            }
        }

        void Slave::remove_framework(Framework *framework) {

            CHECK_NOTNULL(framework);

            LOG(INFO) << "Cleaning up framework " << framework->id().value();

            CHECK(framework->state == Framework::RUNNING ||
                  framework->state == Framework::TERMINATING);

            m_frameworks.erase(framework->id().value());
        }

        void Slave::shutdown_executor(const UPID &from,
                                      const mesos::FrameworkID &frameworkId,
                                      const mesos::ExecutorID &executorId) {
            if (UPID(m_master) != from) {
                LOG(INFO) << "Ignoring shutdown executor message for executor";
                return;
            }

            LOG(INFO) << "Ask to shut down executor " << executorId.value()
                      << " of framework " << frameworkId.value() << " by " << from;

            Framework *framework = get_framework(frameworkId);
            if (framework == nullptr) {
                LOG(INFO) << "Cannot shut down executor " << executorId.value()
                          << " of unknown framework " << frameworkId.value();
            }
            Executor* executor = framework->executors[executorId.value()];
            executor->state = Executor::TERMINATING;
        }


        void Slave::register_feedback(const string &hostname) {
            cout << " receive register feedback from master" << hostname << endl;
        }

        void Slave::finalize() {
            ProcessBase::finalize();
            LOG(INFO) << self() << " finalize()";
        }

        void Slave::heartbeat() {
            send_heartbeat_to_master();
            process::delay(m_interval, self(), &Self::heartbeat);
        }

        void Slave::shutdown(const UPID &master, const ShutdownMessage &shutdown_message) {
            ReplyShutdownMessage reply_message;
            reply_message.set_master_ip(shutdown_message.master_ip());
            reply_message.set_slave_ip(shutdown_message.slave_ip());
            reply_message.set_is_shutdown(true);
            send(master, reply_message);
            terminate(self());
        }

        void Slave::send_heartbeat_to_master() {

            LOG(INFO) << "The Heartbeat is: " << m_interval;

            auto t1 = std::chrono::system_clock::now();

            RuntimeResourcesMessage *rr_message = new RuntimeResourcesMessage();
            rr_message->set_slave_uuid(m_uuid);

            RuntimeResourceUsage::CpuOccupy f_cpu, s_cpu;

            // get cpu usage
            msp_runtime_resource_usage->get_cpu_used_info(&f_cpu);
            usleep(1000000); // we used 1 second = 10^6  microseconds as the default duration for cpu usage calculation
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

        void Slave::reregister_to_master(const UPID &from, const ReregisterMasterMessage &message) {

            LOG(INFO) << "got a ReregisteredMasterMessage from " << from;
            if (message.slave_ip() == stringify(self().address.ip)) {
                m_master = "master@" + message.master_ip() + ":" + message.port();
                msp_masterUPID.reset(new UPID(m_master));
                LOG(INFO) << " prepare to  a  heartbeat to the new master " << m_master << " ";
                send_heartbeat_to_master();
            }
        }
    }
}


using namespace chameleon::slave;

int main(int argc, char *argv[]) {

    chameleon::set_storage_paths_of_glog("slave");// provides the program name
    chameleon::set_flags_of_glog();


    google::SetUsageMessage("usage : Option[name] \n"
                            "--port      the port used by the program \n"
                            "--master    the master ip and port,example:127.0.0.1:8080 \n"
                            "--ht        fixed time interval, slave send message to master \n"
                            "            and the interval >= 2 \n"
                            "--work_dir  the path to store download file");
    google::SetVersionString("Chameleon v1.0");
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::CommandLineFlagInfo info;

    if (master_Str && port_Int) {
        os::setenv("LIBPROCESS_PORT", stringify(FLAGS_port));

        process::initialize("slave");
        Slave slave;
        slave.setM_interval(Seconds(FLAGS_ht));
        slave.setM_work_dir(FLAGS_work_dir);

        string master_ip_and_port = "master@" + stringify(FLAGS_master);
        slave.setM_master(master_ip_and_port);
        PID<Slave> cur_slave = process::spawn(slave);
        LOG(INFO) << "Running slave on " << process::address().ip << ":" << process::address().port;

        const PID<Slave> slave_pid = slave.self();
        process::wait(slave.self());
    } else {
        LOG(INFO) << "To run this program , must set all parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    }

    return 0;
}


