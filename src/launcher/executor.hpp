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
#include <stout/protobuf.hpp>
#include <process/subprocess.hpp>
#include <process/future.hpp>
#include <process/io.hpp>

// libprocess dependenci
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/future.hpp>
#include <process/delay.hpp>
#include <process/subprocess.hpp>
#include <process/clock.hpp>
#include <process/reap.hpp>
// protobuf
#include <messages.pb.h>
#include <mesos.pb.h>

#include <exec.hpp>
#include <chameleon_protobuf_utils.hpp>

using std::string;
using process::Subprocess;
using process::Future;
namespace io = process::io;

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
                const Option<mesos::Environment> _taskEnvironment,
                const mesos::FrameworkID& _frameworkId,
                const mesos::ExecutorID& _executorId);

        virtual ~CommandExecutor() = default;

    protected:
        void initialize();

        /**
         * Function name  : launch
         * Author         : ZhangYixin
         * Description    : launch task
         * Return         : void
         */
        void launch(const mesos::TaskInfo& task);

    private:
        /**
         * Function name  : createTaskStatus
         * Author         : ZhangYixin
         * Description    : Use this helper to create a status update
         * Return         : mesos::TaskStatus
         */
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

        /**
         * Function name  : selfTerminate
         * Author         : ZhangYixin
         * Description    : Use this helper to create a status update
         * Return         : mesos::TaskStatus
         */
        void selfTerminate();

        string get_current_user();

    private:
        pid_t m_pid;
        Option<mesos::TaskID> m_taskId;
        Option<string> m_user;
        Option<string> m_taskCommand;
        Option<mesos::Environment> m_taskEnvironment;
        const mesos::FrameworkID m_frameworkId;
        const mesos::ExecutorID m_executorId;
        chameleon::ChameleonExecutorDriver* m_driver;
        bool launched;
        bool terminated;

        Option<mesos::FrameworkInfo> m_frameworkInfo;
        string m_launcherDir;
        Option<string> m_rootfs;
        Option<string> m_sandboxDirectory;
        Option<string> m_workingDirectory;
    };

    class Flags : public virtual flags::FlagsBase
    {
    public:
        Flags();
        Option<string> m_rootfs;
        Option<string> m_sandbox_directory;
        Option<string> m_working_directory;
        Option<string> m_user;
        Option<string> m_task_command;
        Option<mesos::Environment> m_task_environment;
        string m_launcher_dir;
    };

}

#endif //CHAMELEON_EXECUTOR_H
