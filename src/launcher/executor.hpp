//
// Created by root on 19-1-11.
//

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
#include <process/delay.hpp>
#include <process/subprocess.hpp>

// protobuf
#include <messages.pb.h>
#include <mesos.pb.h>

#include <exec.hpp>

using std::string;

namespace chameleon {
    class ChameleonExecutorDriver;

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

        void launch(const mesos::TaskInfo& task);

    private:

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
