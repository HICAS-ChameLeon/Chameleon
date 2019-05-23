//
// Created by root on 19-1-14.
//

#ifndef CHAMELEON_EXEC_H
#define CHAMELEON_EXEC_H

// C++ 11 dependencies
#include <iostream>
#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/os.hpp>
#include <stout/strings.hpp>
#include <stout/option.hpp>
#include <stout/try.hpp>
#include <stout/uuid.hpp>


// libprocess dependenci
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/delay.hpp>
#include <process/subprocess.hpp>

// protobuf
#include <messages.pb.h>
#include <mesos.pb.h>

namespace chameleon{

    /*
     * className：ExecutorDriver
     * date：19/1/12
     * author：ZhangYixin 1968959287@qq.com
     * description： start Executor.
     */
    class ExecutorDriver
    {
    public:
        // Empty virtual destructor (necessary to instantiate subclasses).
        virtual ~ExecutorDriver() {}

        // Starts the executor driver. This needs to be called before any
        // other driver calls are made.
        virtual mesos::Status start(process::UPID commandExecutorPid) = 0;

        virtual mesos::Status stop() = 0;
        // Stops the executor driver.
        //virtual mesos::Status stop() = 0;

        virtual mesos::Status sendStatusUpdate(const mesos::TaskStatus& status) = 0;
    };

    class ExecutorProcess;

    class ChameleonExecutorDriver : public ExecutorDriver
    {
    public:
        // Creates a new driver that uses the specified Executor. Note, the
        // executor pointer must outlive the driver.
        ChameleonExecutorDriver();

        virtual ~ChameleonExecutorDriver();

        mesos::Status start(process::UPID commandExecutorPid);

        mesos::Status sendStatusUpdate(const mesos::TaskStatus& status);

        mesos::Status stop();

    private:
        friend class chameleon::ExecutorProcess;
        // Libprocess process for communicating with slave.
        ExecutorProcess* m_process;
        // Current status of the driver.
        /*
         * enum Status {
             DRIVER_NOT_STARTED = 1,
             DRIVER_RUNNING = 2,
             DRIVER_ABORTED = 3,
             DRIVER_STOPPED = 4
            };
         * */
        mesos::Status m_status;

        process::UPID m_commandExecutor;

    };


    class ExecutorProcess : public ProtobufProcess<ExecutorProcess>{
    public:
        ExecutorProcess(
                const process::UPID& _slave,
                ChameleonExecutorDriver* _driver,
                const mesos::SlaveID& _slaveId,
                const mesos::FrameworkID& _frameworkId,
                const mesos::ExecutorID& _executorId,
                process::UPID& _commandExecutor) ;
    protected:
        virtual void initialize();

        void registered(
                const mesos::ExecutorInfo& executorInfo,
                const mesos::FrameworkID& frameworkId,
                const mesos::FrameworkInfo& frameworkInfo,
                const mesos::SlaveID& slaveId,
                const mesos::SlaveInfo& slaveInfo);

        void runTask(const mesos::TaskInfo& task);

        void sendStatusUpdate(const mesos::TaskStatus& status);

        void stop();

    private:
        friend class chameleon::ChameleonExecutorDriver;

        process::UPID m_slave;
        chameleon::ChameleonExecutorDriver* m_driver;
        mesos::SlaveID m_slaveId;
        mesos::FrameworkID m_frameworkId;
        mesos::ExecutorID m_executorId;

        process::UPID m_commandExecutorId;
        //chameleon::CommandExecutor* m_executor;

        bool local;
        bool connected; // Registered with the slave.
    };


}


#endif //CHAMELEON_EXEC_H
