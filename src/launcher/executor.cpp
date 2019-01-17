//
// Created by root on 19-1-11.
//

#include "executor.hpp"


namespace chameleon {
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
            }

    void CommandExecutor::initialize() {
        std::cout<<"yxxxx CommandExecutor initialize"<<std::endl;
        m_driver->start();
    }

    void CommandExecutor::launch(const mesos::TaskInfo &task) {

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









