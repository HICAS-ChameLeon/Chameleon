/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author      : Heldon 764165887@qq.com
 * Date       ：19-03-06
 * Description：launch a task in docker container
 */
//C++11 denpendencies
#include <iostream>
#include <string>

//google dependencies
#include <gflags/gflags.h>

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
#include <configuration_glog.hpp>

using namespace mesos;

using boost::lexical_cast;  //to transform data type

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::string;
using std::vector;

//docker launch flags
DEFINE_double(cpu, 1, "Cpus per task");
DEFINE_double(mem, 32, "Memory(MB) per task");
DEFINE_string(docker_image, "","The docker image name");
DEFINE_string(master, "", "The master node pid");
DEFINE_string(command, " ", "The docker container entry command");

/**
 * Function name  : ValidateStr
 * Author         : Heldon
 * Date           : 2018-12-13
 * Description    : Determines whether the input parameter is valid
 * Return         : True or False*/
static bool ValidateStr(const char *flagname, const string &value) {
    if (!value.empty()) {
        return true;
    }
    printf("Invalid value for --%s: To run this program, you must set a meaningful value for it "
           "%s\n", flagname, value.c_str());;
    return false;
}
static const bool docker_image_Str = gflags::RegisterFlagValidator(&FLAGS_docker_image, &ValidateStr);
static const bool master_Str = gflags::RegisterFlagValidator(&FLAGS_docker_image, &ValidateStr);

class DockerNoExecutorScheduler : public Scheduler
{
public:
  DockerNoExecutorScheduler()
    : tasksLaunched(0), tasksFinished(0), totalTasks(1) {}

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
      while (tasksLaunched < totalTasks && cpus >= FLAGS_cpu && mem >= FLAGS_mem) {
        int taskId = tasksLaunched++;

        cout << "Starting task " << taskId << " on "
             << offer.hostname() << endl;

        TaskInfo task;
        task.set_name("Task " + lexical_cast<string>(taskId));
        task.mutable_task_id()->set_value(lexical_cast<string>(taskId));
        task.mutable_slave_id()->MergeFrom(offer.slave_id());
        task.mutable_command()->set_value(FLAGS_command);
        //task.mutable_command()->set_shell(false);

        // Use Docker to run the task.
        ContainerInfo containerInfo;
        containerInfo.set_type(ContainerInfo::DOCKER);

        ContainerInfo::DockerInfo dockerInfo;
        //dockerInfo.set_image("alpine");
        dockerInfo.set_image(FLAGS_docker_image);

        containerInfo.mutable_docker()->CopyFrom(dockerInfo);
        task.mutable_container()->CopyFrom(containerInfo);

        Resource* resource;

        resource = task.add_resources();
        resource->set_name("cpus");
        resource->set_type(Value::SCALAR);
        resource->mutable_scalar()->set_value(FLAGS_cpu);  //--cpu-shares = max(1 * 1024, 2)
                                                           //--cpu-quota = max(1 * 100ms, 1ms)
        resource = task.add_resources();
        resource->set_name("mem");
        resource->set_type(Value::SCALAR);
        resource->mutable_scalar()->set_value(1024);  //--memory = max(32MB , 32MB)

        tasks.push_back(task);

        cpus -= FLAGS_cpu;
        mem -= FLAGS_mem;
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
  google::SetUsageMessage("usage : Option[name] \n"
                          "--master          The master node pid. example:172.20.110.59:6060\n"
                          "--docker_image    The image name on DockerHub. example:heldon/hello_docker\n"
                          "--cpus            Cpus per task.(default:1)\n"
                          "--mem             Memory(MB) per task.(default:32)\n"
                          "--command         The docker container entry command");
  google::SetVersionString("Chameleon v1.0");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::CommandLineFlagInfo info;

  //--master and --docker_image must exist
  if(!master_Str){
      cerr<< "Wrong argument"<<endl;
      return -1;
  } else if(!docker_image_Str){
      cerr<< "Wrong argument"<<endl;
      return -1;
  }

  //construct scheduler
  DockerNoExecutorScheduler scheduler;

  FrameworkInfo framework;
  framework.set_user(""); // Have Mesos fill in the current user.
  framework.set_name("Docker Launch (C++)");
  framework.set_checkpoint(true);

  MesosSchedulerDriver* driver;
  driver = new MesosSchedulerDriver(&scheduler, framework, FLAGS_master);

  int status = driver->run() == DRIVER_STOPPED ? 0 : 1;
  LOG(INFO)<<"Heldon framework name : " << framework.name();

  // Ensure that the driver process terminates.
  driver->stop();

  delete driver;
  return status;
}
