/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */

//#include <stout/flags.hpp>
#include <messages.pb.h>
#include "slave.hpp"
//#include "slave_flags.hpp"
//#include "containerizer/docker.hpp"

//The following flags has default values
DEFINE_uint32(ht, 6, "Heartbeat interval");
DEFINE_int32(port, 6061, "port");

//The following flags must be set by user
DEFINE_string(master, "", "master ip and port info");
DEFINE_string(work_dir, "work_dir",
              "the path to store the files of frameworks. The default is build/src/slave/work_dir");

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

    Slave::Slave() : ProcessBase("slave"), m_interval() {
        msp_resource_collector = make_shared<ResourceCollector>(ResourceCollector());
        msp_runtime_resource_usage = make_shared<RuntimeResourceUsage>(RuntimeResourceUsage());
//        setting::SLAVE_EXE_DIR = os::getcwd();
        m_cwd = os::getcwd();
        m_software_resource_manager = new SoftwareResourceManager(m_cwd, m_cwd+"/public_resources");

//            msp_resource_collector = new ResourceCollector();
    }

    Slave::~Slave() {
        delete hr_message;

        delete m_software_resource_manager;
        LOG(INFO) << "~Slave";
    }


    void Slave::initialize() {
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        m_uuid = UUID::random().toString();

//        LOG(INFO) << "slave executable path" << setting::SLAVE_EXE_DIR;

        msp_masterUPID = make_shared<UPID>(UPID(m_master));

        m_slaveInfo.set_hostname(self().address.hostname().get());
        m_slaveInfo.mutable_id()->set_value(m_uuid);
        m_slaveID.set_value(m_uuid);
        m_slaveInfo.set_port(self().address.port);

        //install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);
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

//        install<mesos::internal::ShutdownExecutorMessage>(
//                &Slave::shutdownExecutor,
//                &mesos::internal::ShutdownExecutorMessage::framework_id,
//                &mesos::internal::ShutdownExecutorMessage::executor_id);

        install<mesos::internal::ShutdownFrameworkMessage>(
                &Slave::shutdown_framework,
                &mesos::internal::ShutdownFrameworkMessage::framework_id);

        install<ReregisterMasterMessage>(&Slave::reregister_to_master);

        install<LaunchMasterMessage>(&Slave::launch_master);

        // http://172.20.110.228:6061/slave/runtime-resources
        route(
                "/runtime-resources",
                "get the runtime resources of the current slave",
                [this](Request request) {
                    JSON::Object result = JSON::protobuf(m_runtime_resources);
                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
                });

        // send the information fo hardware resources on this machine to master
        hr_message = msp_resource_collector->collect_hardware_resources();
        DLOG(INFO) << *msp_masterUPID;
        string slave_id = stringify(self().address.ip);
        hr_message->set_slave_id(slave_id);

        hr_message->set_slave_uuid(m_uuid);
        hr_message->set_slave_hostname(self().address.hostname().get());
        DLOG(INFO) << "Before send message to master";

        send(*msp_masterUPID, *hr_message);

        LOG(INFO) << "The initialization of slave itself finished.";
        LOG(INFO) << self() << " starts to send heartbeat message to the master";
        heartbeat();

    }

    /**
     * change the spark shell value of the specified command in the task
     * @param spark_home_path spark_home_path = da88dffc-19bf-47ea-b061-8dc4c16b4d46-0000/spark-2.3.0-bin-hadoop2.7
     *  @param task
     */
    void Slave::modify_command_info_of_running_task(const string &spark_home_path, mesos::TaskInfo &task) {
        if (task.has_command()) {
            mesos::CommandInfo *new_command_info = new mesos::CommandInfo(task.command());
            const string shell_value = task.command().value();
            auto it = shell_value.find("spark-class");
            if (it != string::npos) {
                const string right_part = shell_value.substr(it);
                const string left_part = " \"" + spark_home_path + "/bin/";
                const string final_value = left_part + right_part;
                new_command_info->set_value(final_value);
                task.clear_command();
                task.set_allocated_command(new_command_info);
                LOG(INFO) << "the final value of command shell is " << final_value;
            }
        }
    }

    /**
     * Funtion  : runTask
     * Author   : weiguow
     * Date     : 2019-1-2
     * */
    void Slave:: run_task(
            const process::UPID &from,
            const mesos::FrameworkInfo &frameworkInfo,
            const mesos::FrameworkID &frameworkId,
            const process::UPID &pid,
            const mesos::TaskInfo &taskInfo) {
        LOG(INFO) << "Get task from master, start the mesos executor first";
        const mesos::ExecutorInfo executorInfo = get_executorinfo(frameworkInfo, taskInfo);

        Option<process::UPID> frameworkPid = None();

        Framework *framework = get_framework(frameworkId);
        framework = new Framework(this, frameworkInfo, frameworkPid);

        frameworks[frameworkId.value()] = framework;

        LOG(INFO) << "Start executor on framework " << framework->id().value();

//        const string current_cwd = os::getcwd();
        const string sanbox_path = path::join(m_work_dir, frameworkId.value());
        Try<Nothing> mkdir_sanbox = os::mkdir(sanbox_path);
        if (mkdir_sanbox.isError()) {
            LOG(ERROR) << mkdir_sanbox.error();
            return;
        }

        // change the spark home of the command information if the running task belongs to spark
        // spark_home_path = sanbox_path + "spark-2.3.0-bin-hadoop2.7"
        // for example,
        // sanbox_path = Chameleon/build/src/slave/da88dffc-19bf-47ea-b061-8dc4c16b4d46-0000
        //  spark_home_path = da88dffc-19bf-47ea-b061-8dc4c16b4d46-0000/spark-2.3.0-bin-hadoop2.7
        const string spark_home_path = path::join(sanbox_path, "spark-2.3.0-bin-hadoop2.7");
        mesos::TaskInfo copy_task(taskInfo);
        modify_command_info_of_running_task(spark_home_path, copy_task);

        // queue the task and executor_info
        m_tasks.push(copy_task);
        m_executorInfo = executorInfo;
        if(taskInfo.container().type() == mesos::ContainerInfo::MESOS){
            process::Future<Nothing> download_result;
            Promise<Nothing> promise;
            if (!os::exists(spark_home_path)) {
                LOG(INFO) << "spark  didn't exist, download it frist";
                mesos::fetcher::FetcherInfo *fetcher_info = new mesos::fetcher::FetcherInfo();
                mesos::fetcher::FetcherInfo_Item *item = fetcher_info->add_items();
                mesos::fetcher::URI *uri = new mesos::fetcher::URI();
                fetcher_info->set_sandbox_directory(sanbox_path);
                //        http://archive.apache.org/dist/spark/spark-2.3.0/spark-2.3.0-bin-hadoop2.7.tgz
                uri->set_value("http://archive.apache.org/dist/spark/spark-2.3.0/spark-2.3.0-bin-hadoop2.7.tgz");
                item->set_allocated_uri(uri);
                item->set_action(mesos::fetcher::FetcherInfo_Item_Action_BYPASS_CACHE);
                download_result = m_software_resource_manager->download("my_spark", *fetcher_info);
                delete fetcher_info;

            } else {
                LOG(INFO) << "the Spark framework has existed";

                download_result = promise.future();
                promise.set(Nothing());
            }
            download_result.onAny(process::defer(self(), &Self::start_mesos_executor, lambda::_1, framework));
//        start_mesos_executor(framework);
        }
        else if(taskInfo.container().type() == mesos::ContainerInfo::DOCKER){
            start_docker_container(taskInfo, framework);
        }

    }

    void Slave::start_mesos_executor(const Future<Nothing> &future, const Framework *framework) {
        if (!future.isReady()) {
            if (future.isFailed()) {
                LOG(ERROR) << future.failure();
            }
            LOG(ERROR) << "framework " << framework->info.name() << " downloaded failed";
            return;
        } else {
            LOG(INFO) << "framework " << framework->info.name() << " downloaded successfully";
        }

        LOG(INFO)<<"Heldon enter function start_mesos_executor";
        const string slave_upid = construct_UPID_string("slave", stringify(self().address.ip), "6061");
        const string mesos_directory = path::join(os::getcwd(), "/mesos_executor/mesos-directory");

        const std::map<string, string> environment =
                {
                        {"MESOS_FRAMEWORK_ID", framework->id().value()},
                        {"MESOS_EXECUTOR_ID",  m_executorInfo.executor_id().value()},
                        {"MESOS_SLAVE_PID",    slave_upid},
                        {"MESOS_SLAVE_ID",     m_slaveInfo.id().value()},
                        {"MESOS_DIRECTORY",    mesos_directory},
                        {"MESOS_CHECKPOINT",   "0"}
                };

        const string mesos_executor_path = path::join(os::getcwd(), "/mesos_executor/mesos-executor");
        LOG(INFO) << "start mesos executor finished ";
        Try<Subprocess> child = subprocess(
                mesos_executor_path,
                Subprocess::FD(STDIN_FILENO),
                Subprocess::FD(STDOUT_FILENO),
                Subprocess::FD(STDERR_FILENO),
                environment
        );
        if (child.isError()) {
            LOG(INFO) << child.error();
        }
    }

    /**
    * Function name  : start_docker_container
    * Author         : Heldon
    * Date           : 2019-03-12
    * Description    : start the task on docker
    * Return         : void
    */
    void Slave::start_docker_container(const mesos::TaskInfo& taskInfo, const Framework *framework){


        const string slave_upid = construct_UPID_string("slave", stringify(self().address.ip), "6061");
        const std::map<string, string> environment =
                {
                        {"MESOS_FRAMEWORK_ID", framework->id().value()},
                        {"MESOS_EXECUTOR_ID",  m_executorInfo.executor_id().value()},
                        {"MESOS_SLAVE_PID",    slave_upid},
                        {"MESOS_SLAVE_ID",     m_slaveInfo.id().value()},
                        {"MESOS_CHECKPOINT",   "0"}
                };

        //Compare resources and if executorInfo's resources are different from taskInfo's
        //then add it to resources
        mesos::Resources resources = m_executorInfo.resources();

        if (taskInfo.resources().size()) {
            resources += taskInfo.resources();
        }

        m_executorInfo.mutable_resources()->CopyFrom(resources);

        //launch the container
        mesos::ContainerID container_id;
        container_id.set_value(UUID::random().toString());

        const string container_directory = path::join(os::getcwd(), "/container/"+container_id.value());
        cout<<container_directory<<endl;
        Future<bool> launch;

        LOG(INFO)<<"Heldon enter function containerizer->launch";
        LOG(INFO)<<"Heldon m_executorInfo.resources().size() : "<<m_executorInfo.resources().size();

        launch = m_containerizer->launch(
                container_id,
                taskInfo,
                m_executorInfo,
                container_directory,
                "heldon",
                m_slaveInfo.id(),
                environment);
    }

    void Slave::register_executor(const UPID &from,
                                 const mesos::FrameworkID &frameworkId,
                                 const mesos::ExecutorID &executorId) {
        LOG(INFO) << "Got registration for executor '" << executorId.value()
                  << "' of framework " << frameworkId.value() << " from "
                  << stringify(from);

        Framework *framework = get_framework(frameworkId);

        mesos::internal::ExecutorRegisteredMessage message;
        message.mutable_executor_info()->mutable_framework_id()->MergeFrom(framework->id());
        message.mutable_executor_info()->MergeFrom(m_executorInfo);
        message.mutable_framework_id()->MergeFrom(framework->id());
        message.mutable_framework_info()->MergeFrom(framework->info);
        message.mutable_slave_id()->MergeFrom(m_slaveInfo.id());
        message.mutable_slave_info()->MergeFrom(m_slaveInfo);
        send(from, message);

        mesos::internal::RunTaskMessage run_task_message;
        run_task_message.mutable_framework()->MergeFrom(framework->info);
        if (!m_tasks.empty()) {
            mesos::TaskInfo current_task = m_tasks.front();
            run_task_message.mutable_task()->MergeFrom(current_task);
            m_tasks.pop();
        } else {
            LOG(FATAL) << " No task left for new executor to run ";
        }
        run_task_message.set_pid(from);

        send(from, run_task_message);
    }


    /**
     * Function  : getExecutorInfo
     * Author    : weiguow
     * Date      : 2019-1-4
     * Description  : getExecutorInfo from FrameworkInfo & TaskInfo
     * */
    mesos::ExecutorInfo Slave::get_executorinfo(
            const mesos::FrameworkInfo &frameworkInfo,
            const mesos::TaskInfo &task) const {

        if (task.has_executor()) {
            return task.executor();
        }

        mesos::ExecutorInfo executorInfo;

        // Command executors share the same id as the task.
        executorInfo.mutable_executor_id()->set_value(task.task_id().value());
        LOG(INFO) << " generate new executorInfo, its executor_id is " << executorInfo.mutable_executor_id()->value();
        executorInfo.mutable_framework_id()->CopyFrom(frameworkInfo.id());

        if (task.has_container()) {
            executorInfo.mutable_container()->CopyFrom(task.container());
            LOG(INFO)<<"Heldon task has a container" ;
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

    /**
     * Function    : status_update
     * Author      : weiguow
     * Date        : 2019-1-8
     * Description : Encapsulates the statusUpdate message and send ack to executor,
     * also send status update message to master
     * @param      : update & pid
     * */
    void Slave::status_update(mesos::internal::StatusUpdate update, const Option<UPID> &pid) {

        LOG(INFO) << "Received status update " << update.status().state()
          << " of framework " << update.framework_id().value();

        update.mutable_status()->set_uuid(update.uuid());
        update.mutable_status()->set_source(
                pid == UPID() ? mesos::TaskStatus::SOURCE_SLAVE : mesos::TaskStatus::SOURCE_EXECUTOR);
        update.mutable_status()->mutable_executor_id()->CopyFrom(update.executor_id());


        LOG(INFO) << "Sending task update " << update.status().state()
                  << " to " << m_master;

        mesos::internal::StatusUpdateMessage status_update_message;
        status_update_message.mutable_update()->MergeFrom(update);
        status_update_message.set_pid(self());
        send(m_master, status_update_message);


        mesos::internal::StatusUpdateAcknowledgementMessage message;
        message.mutable_framework_id()->MergeFrom(update.framework_id());
        message.mutable_slave_id()->MergeFrom(update.slave_id());
        message.mutable_task_id()->MergeFrom(update.status().task_id());
        message.set_uuid(update.uuid());

        if (pid.isSome()) {
            LOG(INFO) << "Sending acknowledgement for status update " << update.status().state()
                      << " to " << pid.get();  //executor(1)@172.20.110.77:39343
            send(pid.get(), message);
        } else {
            LOG(INFO) << "Ignoring update status ";
        }


    }

    /**
     * Function     : statusUpdateAcknowledgement
     * Author      : weiguow
     * Date        : 2019-1-10
     * Description : get statusUpdateAcknowledgement message from master to
     *               make sure the status update is successful
     * @param      : from, slaveId, frameworkId, taskId, uuid
     * */
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
        LOG(INFO) << "Status update manager successfully of framework " << frameworkId.value();
    }

    /**
     * get FrameworkInfo by FrameworkId-by weiguow-2019/2/25
     * */
    Framework *Slave::get_framework(const mesos::FrameworkID &frameworkId) const {
        if (frameworks.count(frameworkId.value()) > 0) {
            return frameworks.at(frameworkId.value());
        }
        return nullptr;
    }

    /**
     * Function     : shutdown_framework
     * Author       : weiguow
     * Date         : 2019-2-26
     * Description  : shutdownFramework after task run over
     * */
    void Slave::shutdown_framework(const process::UPID &from, const mesos::FrameworkID &frameworkId) {
        LOG(INFO) << "Asked to shut down framework " << frameworkId.value()
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

                LOG(INFO) << "Cleaning up framework " << framework->id().value();

                CHECK(framework->state == Framework::RUNNING ||
                      framework->state == Framework::TERMINATING);

                frameworks.erase(framework->id().value());

                break;
            default:
                break;
        }
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

        LOG(INFO) << "The Heartbeat interval is: " << m_interval;

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
        // update current runtime resources of the current slave
        m_runtime_resources.CopyFrom(*rr_message);
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
            UPID new_master_ip(m_master);
            DLOG(INFO) << "Before send message to master";
            send(new_master_ip, *hr_message);
            send_heartbeat_to_master();
        }
    }

    void Slave::setM_containerizer(slave::DockerContainerizer *m_containerizer) {
        Slave::m_containerizer = m_containerizer;
    }

    /**
     *     launch master by subprocess
     *  lele, makun
     * @param super_master
     * @param message
     */
    void Slave::launch_master(const UPID &super_master, const LaunchMasterMessage &message) {
        LOG(INFO) << self().address << " received message from " << super_master;
//        string launch_command = "/home/marcie/chameleon/Chameleon1/Chameleon/build/src/master/master --webui_path=/home/lemaker/open-source/Chameleon/src/webui";
        string launch_command = //"valgrind --tool=memcheck --leak-check=full --track-origins=yes --leak-resolution=high --show-reachable=yes --log-file=memchecklog" +
                message.master_path() + " --webui_path=" + message.webui_path();
        const string stdoutPath = path::join(m_cwd, "stdout");
        Try<int_fd> out = os::open(
                stdoutPath,
                O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK | O_CLOEXEC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (out.isError()) {
            LOG(INFO) << "Failed to create 'stdout' file: " + stdoutPath + " . " +out.error();
        }
        string stderrPath = path::join(m_cwd, "stderr");
        Try<int_fd> err = os::open(
                stderrPath,
                O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK | O_CLOEXEC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (err.isError()) {
            os::close(out.get());
            LOG(INFO) << "Failed to create 'stderr' file: " + err.error();
        }
        Try<Subprocess> s = subprocess(
                launch_command,
                Subprocess::FD(STDIN_FILENO),
                Subprocess::FD(out.get(), Subprocess::IO::OWNED),
                Subprocess::FD(err.get(), Subprocess::IO::OWNED));
        if (s.isError()) {
            LOG(ERROR) << "cannot launch master "<< self().address.ip << ":6060";
            send(super_master,"error");
        }
        LOG(INFO) << self().address.ip << ":6060 launched master successfully.";
        send(super_master,"successed");
    }

//    void Slave::received_new_master(const UPID& from, const MasterRegisteredMessage& message) {
//        LOG(INFO) << "MAKUN " << self().address.ip << " received new master ip from " << from;
//    }

}


using namespace chameleon;

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

        os::setenv("LIBPROCESS_PORT", stringify(FLAGS_port));

//        os::setenv("LIBPROCESS_PORT", stringify(FLAGS_port));  // LIBPROCESS_
        LOG(INFO)<<"Heldon env port : "<< os::getenv("LIBPROCESS_PORT").get();
        process::initialize("slave");

        chameleon::Slave slave;

        Try<chameleon::slave::DockerContainerizer*> docker_containerizer = chameleon::slave::DockerContainerizer::create();
        if (docker_containerizer.isError()) {
            EXIT(EXIT_FAILURE)
                    << "Failed to create a containerizer: " << docker_containerizer.error();
        }

        slave.setM_containerizer(docker_containerizer.get());

        LOG(INFO)<<"Heldon port : "<<stringify(FLAGS_port);
        LOG(INFO)<<"Heldon address.port : " << process::address().port;
        slave.setM_interval(Seconds(FLAGS_ht));
        slave.setM_work_dir(work_dir_path);

        string master_ip_and_port = "master@" + stringify(FLAGS_master);
        slave.setM_master(master_ip_and_port);
        PID<chameleon::Slave> cur_slave = process::spawn(slave);
        LOG(INFO) << "Running slave on " << process::address().ip << ":" << process::address().port;
        const PID<chameleon::Slave> slave_pid = slave.self();
        LOG(INFO) << slave_pid;
        process::wait(slave.self());

        delete docker_containerizer.get();

    } else {
        LOG(INFO) << "To run this program , must set all parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    }

    return 0;
}


