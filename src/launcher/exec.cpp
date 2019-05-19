//
// Created by root on 19-1-14.
//

#include "exec.hpp"

using std::string;


namespace chameleon{

    std::ostream& operator<<(std::ostream& stream, const mesos::SlaveID& slaveId)
    {
        return stream << slaveId.value();
    }
    inline std::ostream& operator<<(std::ostream& stream, const mesos::TaskID& taskId)
    {
        return stream << taskId.value();
    }

    ChameleonExecutorDriver::ChameleonExecutorDriver(): m_process(nullptr), m_status(mesos::DRIVER_NOT_STARTED)
    {
        process::initialize();
    }

    ChameleonExecutorDriver::~ChameleonExecutorDriver() {}

    mesos::Status ChameleonExecutorDriver::start(process::UPID commandExecutorPid) {
            LOG(INFO)<<"ChameleonExecutorDriver start";
            m_commandExecutor = commandExecutorPid;
            if (m_status != mesos::DRIVER_NOT_STARTED) {
                return m_status;
            }
            process::UPID slave;
            mesos::SlaveID slaveId;
            mesos::FrameworkID frameworkId;
            mesos::ExecutorID executorId;


            LOG(INFO)<<"commandExecutorPid:  "<<m_commandExecutor;

            Option<string> value;

            // Get slave PID from environment.
            value = os::getenv("MESOS_SLAVE_PID");
            if (value.isNone()) {
                EXIT(EXIT_FAILURE)
                        << "Expecting 'MESOS_SLAVE_PID' to be set in the environment";
            }

            slave = process::UPID(value.get());

            LOG(INFO)<<"slaveUPID"<<slave;

            CHECK(slave) << "Cannot parse MESOS_SLAVE_PID '" << value.get() << "'";

            // Get slave ID from environment.
            value = os::getenv("MESOS_SLAVE_ID");
            if (value.isNone()) {
                EXIT(EXIT_FAILURE)
                        << "Expecting 'MESOS_SLAVE_ID' to be set in the environment";
            }
            slaveId.set_value(value.get());

            LOG(INFO)<<"slaveId"<<slaveId;
            // Get framework ID from environment.
            value = os::getenv("MESOS_FRAMEWORK_ID");
            if (value.isNone()) {
                EXIT(EXIT_FAILURE)
                        << "Expecting 'MESOS_FRAMEWORK_ID' to be set in the environment";
            }
            frameworkId.set_value(value.get());

            // Get executor ID from environment.
            value = os::getenv("MESOS_EXECUTOR_ID");
            if (value.isNone()) {
                EXIT(EXIT_FAILURE)
                        << "Expecting 'MESOS_EXECUTOR_ID' to be set in the environment";
            }
            executorId.set_value(value.get());


             m_process = new chameleon::ExecutorProcess(
                    slave,
                    this,
                    slaveId,
                    frameworkId,
                    executorId,
                    m_commandExecutor);

            spawn(m_process);

            return m_status = mesos::DRIVER_RUNNING;

    }

    mesos::Status ChameleonExecutorDriver::sendStatusUpdate(const mesos::TaskStatus &status) {
/*        if (status != DRIVER_RUNNING) {
            return status;
        }*/

        //CHECK(process != nullptr);
        LOG(INFO) << "ChameleonExecutorDriver  sendStatusUpdate" ;
        dispatch(m_process, &ExecutorProcess::sendStatusUpdate, status);

        //return status;
    }

    mesos::Status ChameleonExecutorDriver::stop()
    {
        if (m_status != mesos::DRIVER_RUNNING) {
            return m_status;
        }
        dispatch(m_process, &ExecutorProcess::stop);
        m_status = mesos::DRIVER_STOPPED;
        return m_status;

    }

    ExecutorProcess::ExecutorProcess(const process::UPID &_slave, ChameleonExecutorDriver *_driver,
                                     const mesos::SlaveID &_slaveId, const mesos::FrameworkID &_frameworkId,
                                     const mesos::ExecutorID &_executorId,process::UPID& _commandExecutor)
                                     : ProcessBase("executor"),
                                     m_slave(_slave),
                                     m_driver(_driver),
                                     m_slaveId(_slaveId),
                                     m_frameworkId(_frameworkId),
                                     m_executorId(_executorId),
                                     connected(false),
                                     m_commandExecutorId(_commandExecutor)
                                     {
        install<mesos::internal::ExecutorRegisteredMessage>(
                &ExecutorProcess::registered,
                &mesos::internal::ExecutorRegisteredMessage::executor_info,
                &mesos::internal::ExecutorRegisteredMessage::framework_id,
                &mesos::internal::ExecutorRegisteredMessage::framework_info,
                &mesos::internal::ExecutorRegisteredMessage::slave_id,
                &mesos::internal::ExecutorRegisteredMessage::slave_info);

        install<mesos::internal::RunTaskMessage>(
                &ExecutorProcess::runTask,
                &mesos::internal::RunTaskMessage::task);
    }

    void ExecutorProcess::initialize() {
        LOG(INFO) << "Executor started at: " << self()
                  << " with pid " << getpid();

       // slave = "slave@172.20.110.100:6061" ;
    //    link(slave);
       // std::cout<<"yxxxx ExecutorProcess send a RegisterExecutorMessage to slave"<<std::endl;
        // Register with slave.
        LOG(INFO)<<"ExecutorProcess send a RegisterExecutorMessage to slave ";
        mesos::internal::RegisterExecutorMessage message;
        message.mutable_framework_id()->MergeFrom(m_frameworkId);
        message.mutable_executor_id()->MergeFrom(m_executorId);
        send(m_slave, message);
        LOG(INFO)<<"end send a RegisterExecutorMessage to slave "<<m_slave;
    }



    void ExecutorProcess::registered(const mesos::ExecutorInfo &executorInfo, const mesos::FrameworkID &frameworkId,
                                     const mesos::FrameworkInfo &frameworkInfo, const mesos::SlaveID &slaveId,
                                     const mesos::SlaveInfo &slaveInfo) {
        LOG(INFO) << "Executor registered on agent " << slaveId;
        connected = true;
    }

    void ExecutorProcess::runTask(const mesos::TaskInfo &task) {
        LOG(INFO) << "ExecutorProcess runTask " ;
        if (!connected) {
            VLOG(1) << "Ignoring run task message for task " << task.task_id()
                    << " because the driver is disconnected!";
            return;
        }
      //  std::cout<<"yxxxxxxx ExecutorProcess begin runTask "<<slaveId<<std::endl;
       // LOG(INFO) << "yxxxxxxx ExecutorProcess runTask " << slaveId;
//        LOG(INFO) << "yxxxxxxx Executor asked to run task '" << task.task_id() << "'"<<"on"<<commandExecutor;

        send(m_commandExecutorId, task);
    }


    inline std::ostream& operator<<(std::ostream& stream, const mesos::FrameworkID& frameworkId)
    {
        return stream << frameworkId.value();
    }
    std::ostream& operator<<(std::ostream& stream, const mesos::internal::StatusUpdate& update)
    {
        stream << update.status().state();

        if (update.has_uuid()) {
            stream << " (UUID: " << stringify(UUID::fromBytes(update.uuid()).get())
                   << ")";
        }

        stream << " for task " << update.status().task_id();

        if (update.status().has_healthy()) {
            stream << " in health state "
                   << (update.status().healthy() ? "healthy" : "unhealthy");
        }

        return stream << " of framework " << update.framework_id();
    }



    void ExecutorProcess::sendStatusUpdate(const mesos::TaskStatus &status) {

        mesos::internal::StatusUpdateMessage message;
        mesos::internal::StatusUpdate* update = message.mutable_update();
        update->mutable_framework_id()->MergeFrom(m_frameworkId);
        update->mutable_executor_id()->MergeFrom(m_executorId);
        update->mutable_slave_id()->MergeFrom(m_slaveId);
        update->mutable_status()->MergeFrom(status);
        update->set_timestamp(process::Clock::now().secs());
        update->mutable_status()->set_timestamp(update->timestamp());
        message.set_pid(self());

        // We overwrite the UUID for this status update, however with
        // the HTTP API, the executor will have to generate a UUID
        // (which needs to be validated to be RFC-4122 compliant).
        UUID uuid = UUID::random();
        update->set_uuid(uuid.toBytes());
        update->mutable_status()->set_uuid(uuid.toBytes());

        // We overwrite the SlaveID for this status update, however with
        // the HTTP API, this can be overwritten by the slave instead.
        update->mutable_status()->mutable_slave_id()->CopyFrom(m_slaveId);

        LOG(INFO) << "Executor sending status update " << *update;

     // Capture the status update.
        send(m_slave, message);
        LOG(INFO) << "ExecutorProcess  sendStatusUpdate to "<<m_slave;
    }

    void ExecutorProcess::stop()
    {
        terminate(self());
    }
}
