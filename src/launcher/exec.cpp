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



    ChameleonExecutorDriver::ChameleonExecutorDriver() {

    }

    ChameleonExecutorDriver::~ChameleonExecutorDriver() {}

    mesos::Status ChameleonExecutorDriver::start() {
            std::cout<<"yxxxx ChameleonExecutorDriver start"<<std::endl;
/*            if (status != mesos::DRIVER_NOT_STARTED) {
                return status;
            }*/
            process::UPID slave;
            mesos::SlaveID slaveId;
            mesos::FrameworkID frameworkId;
            mesos::ExecutorID executorId;

            Option<string> value;

            // Get slave PID from environment.
            value = os::getenv("MESOS_SLAVE_PID");
            if (value.isNone()) {
                EXIT(EXIT_FAILURE)
                        << "Expecting 'MESOS_SLAVE_PID' to be set in the environment";
            }

            slave = process::UPID(value.get());

            std::cout<<"slavePID"<<slave<<std::endl;

            CHECK(slave) << "Cannot parse MESOS_SLAVE_PID '" << value.get() << "'";

            // Get slave ID from environment.
            value = os::getenv("MESOS_SLAVE_ID");
            if (value.isNone()) {
                EXIT(EXIT_FAILURE)
                        << "Expecting 'MESOS_SLAVE_ID' to be set in the environment";
            }
            slaveId.set_value(value.get());

            std::cout<<"slaveId"<<slaveId<<std::endl;

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

           // CHECK(process == nullptr);

            process = new chameleon::ExecutorProcess(
                    slave,
                    this,
                    slaveId,
                    frameworkId,
                    executorId);

            spawn(process);

            return status = mesos::DRIVER_RUNNING;

    }

    mesos::Status ChameleonExecutorDriver::sendStatusUpdate(const mesos::TaskStatus &status) {

    }

    ExecutorProcess::ExecutorProcess(const process::UPID &_slave, ChameleonExecutorDriver *_driver,
                                     const mesos::SlaveID &_slaveId, const mesos::FrameworkID &_frameworkId,
                                     const mesos::ExecutorID &_executorId)
                                     : ProcessBase("executor"),
                                     slave(_slave),
                                     driver(_driver),
                                     slaveId(_slaveId),
                                     frameworkId(_frameworkId),
                                     executorId(_executorId)
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
        LOG(INFO) << "yxxxx Executor started at: " << self()
                  << " with pid " << getpid();

       // slave = "slave@172.20.110.100:6061" ;
    //    link(slave);
        std::cout<<"yxxxx ExecutorProcess send a RegisterExecutorMessage to slave"<<std::endl;
        // Register with slave.
        LOG(INFO)<<" yxxxx ExecutorProcess send a RegisterExecutorMessage to slave ";
        mesos::internal::RegisterExecutorMessage message;
        message.mutable_framework_id()->MergeFrom(frameworkId);
        message.mutable_executor_id()->MergeFrom(executorId);
        send(slave, message);
        LOG(INFO)<<" yxxxx end send a RegisterExecutorMessage to slave "<<slave;
    }



    void ExecutorProcess::registered(const mesos::ExecutorInfo &executorInfo, const mesos::FrameworkID &frameworkId,
                                     const mesos::FrameworkInfo &frameworkInfo, const mesos::SlaveID &slaveId,
                                     const mesos::SlaveInfo &slaveInfo) {
        std::cout<<"yxxxxxxx Executor registered on agent "<<slaveId<<std::endl;
        LOG(INFO) << "yxxxxxxx Executor registered on agent " << slaveId;
    }

    void ExecutorProcess::runTask(const mesos::TaskInfo &task) {

/*        std::cout<<"yxxxxxxx ExecutorProcess begin runTask "<<slaveId<<std::endl;
        LOG(INFO) << "yxxxxxxx ExecutorProcess runTask " << slaveId;
        LOG(INFO) << "yxxxxxxx Executor asked to run task '" << task.task_id() << "'";*/

       // executor->launch(task);
    }

    void ExecutorProcess::sendStatusUpdate(const mesos::TaskStatus &status) {

    }
}
