#ifndef CHAMELEON_EXECUTOR_H
#define CHAMELEON_EXECUTOR_H

// C++ 11 dependencies
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/os.hpp>
#include <stout/strings.hpp>
#include <stout/flags.hpp>
#include <stout/option.hpp>
#include <stout/try.hpp>


// libprocess dependenci
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/future.hpp>
#include <process/delay.hpp>
#include <process/subprocess.hpp>
#include <process/clock.hpp>
// protobuf
#include <messages.pb.h>
#include <mesos.pb.h>
#include <exec.hpp>

#include <chameleon_protobuf_utils.hpp>

using std::string;

namespace chameleon {
    class ChameleonExecutorDriver;

    /**
      * className：CommandExecutor
      * date：19/1/11
      * author：ZhangYixin 1968959287@qq.com
      * description： start Executor.
    */
    class CommandExecutor : public ProtobufProcess<CommandExecutor> {
    public:
        CommandExecutor(
                const string& _launcherDir,
                const Option<string>& _rootfs,
                const Option<string>& _sandboxDirectory,
                const Option<string>& _workingDirectory,
                const Option<string>& _user,
                const Option<string>& _taskCommand,
                const mesos::FrameworkID& _frameworkId,
                const mesos::ExecutorID& _executorId);

        virtual ~CommandExecutor() = default;

        void initialize();

        /**
         * Function name  : launch
         * Author         : ZhangYixin
         * Description    : launch task
         * Return         : void
         */
        void launch(const mesos::TaskInfo& task);

        mesos::TaskStatus createTaskStatus(
                const mesos::TaskID& _taskId,
                const mesos::TaskState& state,
                const Option<mesos::TaskStatus::Reason>& reason,
                const Option<string>& message);

        mesos::TaskStatus createTaskStatus(
                const mesos::TaskID& _taskId,
                const mesos::TaskState& state);

        void forward(const mesos::TaskStatus& status);

        void reaped(pid_t pid, const process::Future<Option<int>>& status_);

        void selfTerminate();

    private:
        pid_t pid;
        Option<mesos::FrameworkInfo> frameworkInfo;
        Option<mesos::TaskID> taskId;
        string launcherDir;
        Option<string> rootfs;
        Option<string> sandboxDirectory;
        Option<string> workingDirectory;
        Option<string> user;
        Option<string> taskCommand;
        Option<mesos::Environment> taskEnvironment;

        const mesos::FrameworkID frameworkId;
        const mesos::ExecutorID executorId;

        chameleon::ChameleonExecutorDriver* m_driver;

        bool launched;
        bool terminated;
    };

    class Flags : public virtual flags::FlagsBase
    {
    public:
        Flags();
        Option<string> rootfs;
        Option<string> sandbox_directory;
        Option<string> working_directory;
        Option<string> user;
        Option<string> task_command;
        Option<mesos::Environment> task_environment;
        string launcher_dir;
    };

}

#endif //CHAMELEON_EXECUTOR_H
