//
// Created by root on 19-1-11.
//

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
            const mesos::ExecutorID& _executorId) : ProcessBase("CommandExecutor")
            {
                std::cout<<"yxxxx CommandExecutor Construct start"<<std::endl;

                install<mesos::TaskInfo>(
                        &CommandExecutor::launch);
            }

    void CommandExecutor::initialize() {
        std::cout<<"yxxxx CommandExecutor initialize"<<std::endl;
        process::UPID commandExecutorPid = self();
        std::cout<<commandExecutorPid<<std::endl;
        m_driver = new ChameleonExecutorDriver();
        m_driver->start(commandExecutorPid);
    }

/*
    pid_t CommandExecutor::launchTaskSubprocess(
            const mesos::CommandInfo& command,
            const string& launcherDir,
            const mesos::Environment& environment,
            const Option<string>& user,
            const Option<string>& rootfs,
            const Option<string>& sandboxDirectory,
            const Option<string>& workingDirectory){

//        string commandString = strings::format(
//                "%s %s <POSSIBLY-SENSITIVE-DATA>",
//                path::join(launcherDir, MESOS_CONTAINERIZER),
//                MesosContainerizerLaunch::NAME).get();

*/
/*        std::cout << "yxxxxx  Running '" << commandString << "'" << std::endl;

        Try<process::Subprocess> s = subprocess(
                path::join(launcherDir, MESOS_CONTAINERIZER),
                process::Subprocess::FD(STDIN_FILENO),
                process::Subprocess::FD(STDOUT_FILENO),
                process::Subprocess::FD(STDERR_FILENO),
                None(),
                None(),
                {process::Subprocess::ChildHook::SETSID()});*//*



        return s->pid();
    }
*/

    void CommandExecutor::launch(const mesos::TaskInfo &task) {
        LOG(INFO) << "yxxxxxxx CommandExecutor asked to run task '" << task.task_id()<< "'";
       // taskData = TaskData(task);
        taskId = task.task_id();
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
            command = task.command();
        } else {
            LOG(FATAL) << "Expecting task '" << taskId.get() << "' to have a command";
        }

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

        mesos::Environment launchEnvironment;
        foreachvalue (const mesos::Environment::Variable& variable, environment) {
                                launchEnvironment.add_variables()->CopyFrom(variable);
        }

        //--------------------------------------------------

        std::cout << "\n yxxxxxx Starting task " << taskId.get() << std::endl;

/*        pid = launchTaskSubprocess(
                command,
                launcherDir,
                launchEnvironment,
                user,
                rootfs,
                sandboxDirectory,
                workingDirectory);*/
     //   const string *com = command.mutable_value();
        Try<process::Subprocess> exec = process::subprocess(
                *command.mutable_value(),
                process::Subprocess::FD(STDIN_FILENO),
                process::Subprocess::FD(STDOUT_FILENO),
                process::Subprocess::FD(STDERR_FILENO) );

        std::cout << "\n yxxxxx  Forked command at " << pid << std::endl;

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

/*        add(&Flags::task_environment,
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


    std::cout<<"yxxxx CommandExecutor start"<<std::endl;

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









