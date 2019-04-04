//
// Created by root on 19-1-11.
//
#include <process/reap.hpp>
#include <stout/protobuf.hpp>
#include "executor.hpp"

namespace chameleon {
    std::ostream& operator<<(std::ostream& stream, const mesos::TaskID& taskId)
    {
        return stream << taskId.value();
    }

    CommandExecutor::CommandExecutor(
            const string& _launcherDir,
            const Option<string>& _rootfs,
            const Option<string>& _sandboxDirectory,
            const Option<string>& _workingDirectory,
            const Option<string>& _user,
            const Option<string>& _taskCommand,
            const mesos::FrameworkID& _frameworkId,
            const mesos::ExecutorID& _executorId) : ProcessBase("CommandExecutor"),
            taskId(None()),
            executorId(_executorId),
            frameworkId(_frameworkId),
            terminated(false),
            pid(-1),
            launched(false)
            {
                //std::cout<<"yxxxx CommandExecutor Construct start"<<std::endl;
                LOG(INFO) << "CommandExecutor Construct start" ;
                /* use method 'CommandExecutor::launch' to executor TaskInfo */
                install<mesos::TaskInfo>(
                        &CommandExecutor::launch);
            }

    void CommandExecutor::initialize() {
       // std::cout<<"yxxxx CommandExecutor initialize"<<std::endl;
        process::UPID commandExecutorPid = self();
        LOG(INFO) << "CommandExecutor initialize and PID: " <<commandExecutorPid;
    //    std::cout<<commandExecutorPid<<std::endl;
        m_driver = new ChameleonExecutorDriver();
        m_driver->start(commandExecutorPid);
    }



    void CommandExecutor::launch(const mesos::TaskInfo &task) {
        if (launched) {
            LOG(INFO) << "CommandExecutor launched(flase) ";
            mesos::TaskStatus status = CommandExecutor::createTaskStatus(
                    task.task_id(),
                    mesos::TASK_FAILED,
                    None(),
                    "Attempted to run multiple tasks using a \"command\" executor");

            forward(status);
            return;
        }

        taskId = task.task_id();

        LOG(INFO) << "CommandExecutor asked to run task '" << task.task_id()<< "'";

        // Determine the command to launch the task.
        mesos::CommandInfo command;

        //----------------------------------------------------
        if (taskCommand.isSome()) {
            // Get CommandInfo from a JSON string.
            std::cout << "\n taskCommand.isSome " << std::endl;

            Try<JSON::Object> object = JSON::parse<JSON::Object>(taskCommand.get());
            if (object.isError()) {
                ABORT("Failed to parse JSON: " + object.error());
            }

            Try<mesos::CommandInfo> parse = ::protobuf::parse<mesos::CommandInfo>(object.get());

            if (parse.isError()) {
                ABORT("Failed to parse protobuf: " + parse.error());
            }

            command = parse.get();
        } else if (task.has_command()) {
            LOG(INFO) << "task.has_command()" ;
            command = task.command();
        } else {
            LOG(FATAL) << "Expecting task '" << taskId.get() << "' to have a command";
        }


/*        if (command.shell()) {
            CHECK(command.has_value())
            << "Shell command of task '" << taskId.get() << "' is not specified";
        } else {
            CHECK(command.has_value())
            << "Executable of task '" << taskId.get() << "' is not specified";
        }*/

        //-------------------------------------------------------------
        hashmap<string, mesos::Environment::Variable> environment;

        foreachpair (const string& name, const string& value, os::environment()) {
                                mesos::Environment::Variable variable;
                                variable.set_name(name);
                                variable.set_type(mesos::Environment::Variable::VALUE);
                                variable.set_value(value);
                                environment[name] = variable;
                            }

        if (taskEnvironment.isSome()) {
            foreach (const mesos::Environment::Variable& variable,
                     taskEnvironment->variables()) {
                const string& name = variable.name();
                if (environment.contains(name) &&
                    environment[name].value() != variable.value()) {
                    std::cout << "Overwriting environment variable '" << name << "'" << std::endl;
                }
                environment[name] = variable;
            }
        }

        if (command.has_environment()) {
            LOG(INFO) << "-----------   command.has_environment" ;
            foreach (const mesos::Environment::Variable& variable,
                     command.environment().variables()) {
                const string& name = variable.name();
                if (environment.contains(name) &&
                    environment[name].value() != variable.value()) {
                    std::cout << "Overwriting environment variable '" << name << "'" << std::endl;
                }
                environment[name] = variable;
            }
        }
      //  LOG(INFO) << "\n yxxx  environment " << environment.values();

        mesos::Environment launchEnvironment;
        foreachvalue (const mesos::Environment::Variable& variable, environment) {
                                launchEnvironment.add_variables()->CopyFrom(variable);
        }

      //  LOG(INFO) << "\n yxxx launch launchEnvironment " << launchEnvironment.Utf8DebugString();

        const std::map<string, string> environment_string =
                {
                        {"JVM_ARGS", " -Xms3072m -Xmx3072m "},
                        {"_FLINK_CONTAINER_ID",  "taskmanager-00001"},
                        {"FRAMEWORK_NAME",    "Flink"},
                        {"MESOS_EXECUTOR_ID", "taskmanager-00001"},
                        {"TASK_NAME",  "taskmanager-00001"},
                        {"HADOOP_USER_NAME", "zyx"},
                        {"PWD",  "/home/zyx/CLionProjects/Chameleon/cmake-build-debug/src/slave"},
                        {"FLINK_HOME",    "flink"},
                        {"HADOOP_CONF_DIR",     "hadoop/conf"}
                };


       // std::cout << "\n yxxxxxx Starting task " << taskId.get() << std::endl;
        LOG(INFO) << "Starting task " << taskId.get();
        LOG(INFO) << "command :" << *command.mutable_value();
        /*begin run taskInfo*/
        Try<process::Subprocess> exec = process::subprocess(
                *command.mutable_value(),
                process::Subprocess::FD(STDIN_FILENO),
                process::Subprocess::FD(STDOUT_FILENO),
                process::Subprocess::FD(STDERR_FILENO),
                environment_string);

        pid=exec->pid();

        LOG(INFO) << "Forked command at " << pid;

        process::reap(pid)
                .onAny(defer(self(), &Self::reaped, pid, lambda::_1));


        LOG(INFO) << "CommandExecutor createTaskStatus " ;
        mesos::TaskStatus status = CommandExecutor::createTaskStatus(taskId.get(), mesos::TASK_RUNNING);
        forward(status);
        launched = true;

    }


    // Use this helper to create a status update from scratch.
    mesos::TaskStatus CommandExecutor::createTaskStatus(const mesos::TaskID &_taskId, const mesos::TaskState &state,
                                                        const Option<mesos::TaskStatus::Reason> &reason,
                                                        const Option<string> &message) {
        LOG(INFO) << "createTaskStatus 1 " ;
        mesos::TaskStatus status = chameleon::protobuf::createTaskStatus(
                _taskId,
                state,
                UUID::random(),
                process::Clock::now().secs());

        status.mutable_executor_id()->CopyFrom(executorId);
        status.set_source(mesos::TaskStatus::SOURCE_EXECUTOR);

        if (reason.isSome()) {
            status.set_reason(reason.get());
        }

        if (message.isSome()) {
            status.set_message(message.get());
        }

        return status;
    }


    std::ostream& operator<<(std::ostream& stream, const mesos::ExecutorID& executorId)
    {
        return stream << executorId.value();
    }

    // Use this helper to create a status update from scratch.
    mesos::TaskStatus CommandExecutor::createTaskStatus(const mesos::TaskID &_taskId, const mesos::TaskState &state) {
        LOG(INFO) << "createTaskStatus 2 " ;
        mesos::TaskStatus status = chameleon::protobuf::createTaskStatus(
                _taskId,
                state,
                UUID::random(),
                process::Clock::now().secs());
        LOG(INFO) << "createTaskStatus executorId "<<executorId;
        status.mutable_executor_id()->CopyFrom(executorId);
        status.set_source(mesos::TaskStatus::SOURCE_EXECUTOR);

        return status;
    }

    void CommandExecutor::forward(const mesos::TaskStatus &status) {
        LOG(INFO) << "CommandExecutor  forward" ;
        m_driver->sendStatusUpdate(status);
    }

    void CommandExecutor::reaped(pid_t pid, const process::Future<Option<int>>& status_) {
        terminated = true;
        mesos::TaskStatus status = createTaskStatus(
                taskId.get(),
                mesos::TASK_FINISHED);
        forward(status);
       // delay(Seconds(1), self(), &Self::selfTerminate);
        LOG(INFO) << "CommandExecutor  terminate self" ;
        delay(Seconds(2), self(), &Self::selfTerminate);
      //  terminate(self());
       // mesos::TaskState taskState;
      //  string message;
/*
        if (!status_.isReady()) {
            taskState = mesos::TASK_FAILED;
            message =
                    "Failed to get exit status for Command: " +
                    (status_.isFailed() ? status_.failure() : "future discarded");
        } else if (status_.get().isNone()) {
            taskState = mesos::TASK_FAILED;
            message = "Failed to get exit status for Command";
        } else {
            int status = status_.get().get();
            CHECK(WIFEXITED(status) || WIFSIGNALED(status))
            << "Unexpected wait status " << status;

            if (killed) {
                // Send TASK_KILLED if the task was killed as a result of
                // kill() or shutdown().
                taskState = TASK_KILLED;
            } else if (WSUCCEEDED(status)) {
                taskState = TASK_FINISHED;
            } else {
                taskState = TASK_FAILED;
            }

            message = "Command " + WSTRINGIFY(status);
        }

        cout << message << " (pid: " << pid << ")" << endl;

        CHECK_SOME(taskId);

        TaskStatus status = createTaskStatus(
                taskId.get(),
                taskState,
                None(),
                message);


        forward(status);*/
/*
        Option<string> value = os::getenv("MESOS_HTTP_COMMAND_EXECUTOR");
        if (value.isSome() && value.get() == "1") {
            // For HTTP based executor, this is a fail safe in case the agent
            // doesn't send an ACK for the terminal update for some reason.
            delay(Seconds(60), self(), &Self::selfTerminate);
        } else {
            // For adapter based executor, this is a hack to ensure the status
            // update is sent to the agent before we exit the process. Without
            // this we may exit before libprocess has sent the data over the
            // socket. See MESOS-4111 for more details.
            delay(Seconds(1), self(), &Self::selfTerminate);
        }*/
    }

    void CommandExecutor::selfTerminate()
    {
            terminate(self());
    }

    Flags::Flags() {
        add(&Flags::rootfs,
            "rootfs",
            "The path to the root filesystem for the task");

        // The following flags are only applicable when a rootfs is
        // provisioned for this command.
        add(&Flags::sandbox_directory,
            "sandbox_directory",
            "The absolute path for the directory in the container where the\n"
            "sandbox is mapped to");

        add(&Flags::working_directory,
            "working_directory",
            "The working directory for the task in the container.");

        add(&Flags::user,
            "user",
            "The user that the task should be running as.");

        add(&Flags::task_command,
            "task_command",
            "If specified, this is the overrided command for launching the\n"
            "task (instead of the command from TaskInfo).");

        //exist bug  113--158
/*
 *       add(&Flags::task_environment,
            "task_environment",
            "If specified, this is a JSON-ified `Environment` protobuf that\n"
            "should be added to the executor's environment before launching\n"
            "the task.");*/

        add(&Flags::launcher_dir,
            "launcher_dir",
            "Directory path of Mesos binaries.",
            PKGLIBEXECDIR);
    }


}
using namespace chameleon;

int main(int argc, char *argv[]) {
    Flags flags;
    mesos::FrameworkID frameworkId;
    mesos::ExecutorID executorId;

    process::initialize();

    Option<string> value = os::getenv("MESOS_FRAMEWORK_ID");
    if (value.isNone()) {
        EXIT(EXIT_FAILURE)
                << "Expecting 'MESOS_FRAMEWORK_ID' to be set in the environment";
    }
    frameworkId.set_value(value.get());

    value = os::getenv("MESOS_EXECUTOR_ID");
    if (value.isNone()) {
        EXIT(EXIT_FAILURE)
                << "Expecting 'MESOS_EXECUTOR_ID' to be set in the environment";
    }
    executorId.set_value(value.get());


    LOG(INFO)<<"CommandExecutor start";

    process::Owned<chameleon::CommandExecutor> executor(
            new chameleon::CommandExecutor(
                    flags.launcher_dir,
                    flags.rootfs,
                    flags.sandbox_directory,
                    flags.working_directory,
                    flags.user,
                    flags.task_command,
                    frameworkId,
                    executorId));

    process::spawn(executor.get());
    process::wait(executor.get());
    executor.reset();

    return 0;
}









