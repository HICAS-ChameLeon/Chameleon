/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author      : Heldon 764165887@qq.com
 * Date       ：19-03-06
 * Description：launch a task in docker container
 */
//C++11 denpendencies
#include <iostream>
#include <string>

//boost dependencies
#include <boost/lexical_cast.hpp>

//mesos dependencies
#include <mesos/scheduler.hpp>

//stout dependencies
#include <stout/exit.hpp>
#include <stout/option.hpp>
#include <stout/os.hpp>

//proto dependencies
#include <mesos.pb.h>

using namespace mesos;

using boost::lexical_cast;

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::string;
using std::vector;

const int32_t CPUS_PER_TASK = 1;
const int32_t MEM_PER_TASK = 32;

class DockerNoExecutorScheduler : public Scheduler
{
public:
  DockerNoExecutorScheduler()
    : tasksLaunched(0), tasksFinished(0), totalTasks(5) {}

  virtual ~DockerNoExecutorScheduler() {}

  virtual void registered(SchedulerDriver*,
                          const FrameworkID&,
                          const MasterInfo&)
  {
    cout << "Registered!" << endl;
  }

  virtual void reregistered(SchedulerDriver*, const MasterInfo& masterInfo) {}

  virtual void disconnected(SchedulerDriver* driver) {}

  virtual void resourceOffers(SchedulerDriver* driver,
                              const vector<Offer>& offers)
  {
    cout << "." << flush;
    for (size_t i = 0; i < offers.size(); i++){
      LOG(INFO) << "Heldon offers_size : " << offers.size();
      const Offer& offer = offers[i];

      // Lookup resources we care about.
      // TODO(benh): It would be nice to ultimately have some helper
      // functions for looking up resources.
      double cpus = 0;
      double mem = 0;

      for (int i = 0; i < offer.resources_size(); i++) {
        const Resource& resource = offer.resources(i);
        if (resource.name() == "cpus" &&
            resource.type() == Value::SCALAR) {
          cpus = resource.scalar().value();
        } else if (resource.name() == "mem" &&
                   resource.type() == Value::SCALAR) {
          mem = resource.scalar().value();
        }
      }

      // Launch tasks.
      vector<TaskInfo> tasks;
      while (tasksLaunched < totalTasks &&
             cpus >= CPUS_PER_TASK &&
             mem >= MEM_PER_TASK) {
        int taskId = tasksLaunched++;

        cout << "Starting task " << taskId << " on "
             << offer.hostname() << endl;

        TaskInfo task;
        task.set_name("Task " + lexical_cast<string>(taskId));
        task.mutable_task_id()->set_value(lexical_cast<string>(taskId));
        task.mutable_slave_id()->MergeFrom(offer.slave_id());
        task.mutable_command()->set_value("java -jar /hello_docker.jar");
        //task.mutable_command()->set_shell(false);

        // Use Docker to run the task.
        ContainerInfo containerInfo;
        containerInfo.set_type(ContainerInfo::DOCKER);

        ContainerInfo::DockerInfo dockerInfo;
        //dockerInfo.set_image("alpine");
        dockerInfo.set_image("heldon/hello_docker");

        containerInfo.mutable_docker()->CopyFrom(dockerInfo);
        task.mutable_container()->CopyFrom(containerInfo);

        Resource* resource;

        resource = task.add_resources();
        resource->set_name("cpus");
        resource->set_type(Value::SCALAR);
        resource->mutable_scalar()->set_value(CPUS_PER_TASK);  //--cpu-shares = max(1 * 1024, 2)
                                                               //--cpu-quota = max(1 * 100ms, 1ms)
        resource = task.add_resources();
        resource->set_name("mem");
        resource->set_type(Value::SCALAR);
        resource->mutable_scalar()->set_value(MEM_PER_TASK);  //--memory = max(32MB , 32MB)

        tasks.push_back(task);

        cpus -= CPUS_PER_TASK;
        mem -= MEM_PER_TASK;
      }

      driver->launchTasks(offer.id(), tasks);
    }
  }

  virtual void offerRescinded(SchedulerDriver* driver,
                              const OfferID& offerId) {}

  virtual void statusUpdate(SchedulerDriver* driver, const TaskStatus& status)
  {
    int taskId = lexical_cast<int>(status.task_id().value());

    cout << "Task " << taskId << " is in state " << status.state() << endl;

    if (status.state() == TASK_FINISHED)
      tasksFinished++;

    if (tasksFinished == totalTasks)
      driver->stop();
  }

  virtual void frameworkMessage(SchedulerDriver* driver,
                                const ExecutorID& executorId,
                                const SlaveID& slaveId,
                                const string& data) {}

  virtual void slaveLost(SchedulerDriver* driver, const SlaveID& slaveId) {}

  virtual void executorLost(SchedulerDriver* driver,
                            const ExecutorID& executorId,
                            const SlaveID& slaveId,
                            int status) {}

  virtual void error(SchedulerDriver* driver, const string& message) {}

private:
  int tasksLaunched;
  int tasksFinished;
  int totalTasks;
};


int main(int argc, char** argv)
{
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <master>" << endl;
    return -1;
  }

  DockerNoExecutorScheduler scheduler;

  FrameworkInfo framework;
  framework.set_user(""); // Have Mesos fill in the current user.
  framework.set_name("Docker No Executor Framework (C++)");
  framework.set_checkpoint(true);

  MesosSchedulerDriver* driver;
  if (os::getenv("MESOS_AUTHENTICATE_FRAMEWORKS").isSome()) {
    cout << "Enabling authentication for the framework" << endl;

    Option<string> value = os::getenv("DEFAULT_PRINCIPAL");
    if (value.isNone()) {
      EXIT(EXIT_FAILURE)
        << "Expecting authentication principal in the environment";
    }

    Credential credential;
    credential.set_principal(value.get());

    framework.set_principal(value.get());

    value = os::getenv("DEFAULT_SECRET");
    if (value.isNone()) {
      EXIT(EXIT_FAILURE)
        << "Expecting authentication secret in the environment";
    }

    credential.set_secret(value.get());

    driver = new MesosSchedulerDriver(
        &scheduler, framework, argv[1], credential);
  } else {
    framework.set_principal("no-executor-framework-cpp");

    driver = new MesosSchedulerDriver(
        &scheduler, framework, argv[1]);
  }

  int status = driver->run() == DRIVER_STOPPED ? 0 : 1;
  LOG(INFO)<<"Heldon framework name : " << framework.name();

  // Ensure that the driver process terminates.
  driver->stop();

  delete driver;
  return status;
}
