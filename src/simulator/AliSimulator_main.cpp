//
// Created by Heldon on 19-4-12.
//

#include <iostream>
#include <string>
#include <map>

#include <boost/lexical_cast.hpp>

#include <glog/logging.h>
#include <configuration_glog.hpp>

#include <stout/exit.hpp>
#include <stout/option.hpp>
#include <stout/os.hpp>

#include <mesos/scheduler.hpp>

#include "AliTracePreProcessor.h"
#include "AliTraceLoader.hpp"
#include "AliSimulator.hpp"

using boost::lexical_cast;
using namespace mesos;
using namespace AliSim;
const string trace_path = "/home/heldon/chameleon/ali_clusterdata/alibaba_clusterdata_v2018/";

class SimulatorScheduler : public Scheduler{
public:
    //construct function
    SimulatorScheduler() : m_task_launched(0), m_task_finished(0), total_tasks(10){}

    virtual ~SimulatorScheduler() {}

    virtual void registered(SchedulerDriver*, const FrameworkID&, const MasterInfo&){
        cout << "Registered!" << endl;
    }

    virtual void reregistered(SchedulerDriver*, const MasterInfo& masterInfo) {}

    virtual void disconnected(SchedulerDriver* driver) {}

    virtual void resourceOffers(SchedulerDriver* driver, const vector<Offer>& offers){
        AliSim::SimulatedWallTime simulated_wall_time(20);
        AliSim::AliTraceLoader trace_loader(trace_path);
        AliSimulator simulator(simulated_wall_time, trace_loader);
        simulator.Run();
        multimap<uint64_t, Task_identifier> task_map = simulator.Get_task();

        for(size_t i = 0; i < offers.size(); i++){
            LOG(INFO) << "Offers size : " << offers.size();
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
            LOG(INFO) << "CPU : " << cpus << " Memory : " << mem;

            //Launch tasks
            vector<TaskInfo> tasks;
            multimap<uint64_t ,Task_identifier>::iterator it = task_map.begin();
            int task_id = 0;
            for(it; it != task_map.end(); it++){
                task_id++;
                LOG(INFO) << "Starting task " << task_id << " on " << offer.hostname();

                TaskInfo task;
                double plan_cpu = it->second.plan_cpu/100;
                double plan_mem = it->second.plan_mem;
                LOG(INFO) <<"plan_cpu : " << plan_cpu << " plan_mem : " << plan_mem;

                task.set_name("Task " + lexical_cast<string>(task_id));
                task.mutable_task_id()->set_value(lexical_cast<string>(task_id));
                task.mutable_slave_id()->MergeFrom(offer.slave_id());

                Resource* resource;

                resource = task.add_resources();
                resource->set_name("cpus");
                resource->set_type(Value::SCALAR);
                resource->mutable_scalar()->set_value(plan_cpu);

                resource = task.add_resources();
                resource->set_name("mem");
                resource->set_type(Value::SCALAR);
                resource->mutable_scalar()->set_value(plan_mem);

                tasks.push_back(task);

                cpus -= plan_cpu;
                mem -= plan_mem;
            }
            driver->launchTasks(offer.id(), tasks);
        }
    }

    virtual void offerRescinded(SchedulerDriver* driver, const OfferID& offerId) {}

    virtual void statusUpdate(SchedulerDriver* driver, const TaskStatus& status)
    {
        int taskId = lexical_cast<int>(status.task_id().value());

        cout << "Task " << taskId << " is in state " << status.state() << endl;

        if (status.state() == TASK_FINISHED)
            m_task_finished++;

        if (m_task_finished == total_tasks)
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
    int m_task_launched;
    int m_task_finished;
    int total_tasks;
    //AliSim::AliSimulator* m_simulator;
};

int main(int argc, char* argv[]){

    //chameleon::set_storage_paths_of_glog("simulator");// provides the program name
    //chameleon::set_flags_of_glog();
//    AliSim::SimulatedWallTime simulated_wall_time(20);
//    AliSim::AliTraceLoader trace_loader(trace_path);
////    multimap<uint64_t, AliSim::Machine_mega> machine_map;
////    traceLoader.Load_machine(machine_map);
//
//    AliSim::AliSimulator simulator(simulated_wall_time, trace_loader);
//    simulator.Run();
//    multimap<uint64_t, AliSim::Machine_mega> m_machine_mega_map = simulator.Get_machine_mega();
//    multimap<uint64_t, Machine_mega>::iterator it = m_machine_mega_map.begin();
//    while(it != m_machine_mega_map.end()){
//        cout << it->first << " -> " << endl;
//
//        auto count = m_machine_mega_map.count(it->first);
//        for(auto i = 0; i != count; ++it, ++ i){
//            cout << it->second << endl;
//        }
//    }

    SimulatorScheduler sim_scheduler;
    FrameworkInfo framework;
    framework.set_user("heldon"); // Have Mesos fill in the current user.
    framework.set_name("Framework simulator");
    framework.set_checkpoint(true);

    MesosSchedulerDriver* driver;
    driver = new MesosSchedulerDriver(&sim_scheduler, framework, "172.20.110.59:6060");
    int status = driver->run() == DRIVER_STOPPED ? 0 : 1;
    LOG(INFO)<<"Heldon framework name : " << framework.name();

    // Ensure that the driver process terminates.
    driver->stop();

    delete driver;
    return status;
}

