//
// Created by Heldon on 19-4-12.
//

#include <iostream>
#include <string>
#include <map>

#include <glog/logging.h>
#include <configuration_glog.hpp>

#include <stout/exit.hpp>
#include <stout/option.hpp>
#include <stout/os.hpp>

#include <mesos/scheduler.hpp>

#include "AliTracePreProcessor.h"
#include "AliTraceLoader.hpp"
#include "AliSimulator.hpp"

using namespace mesos;
using namespace AliSim;
const string trace_path = "/home/heldon/chameleon/ali_clusterdata/alibaba_clusterdata_v2018/";

class SimulatorScheduler : public Scheduler{
public:
    //construct function
    SimulatorScheduler() : m_task_launched(0), m_task_finished(0), total_tasks(10){
        AliSim::SimulatedWallTime simulated_wall_time(20);
        AliSim::AliTraceLoader trace_loader(trace_path);
        m_simulator = new AliSimulator(simulated_wall_time, trace_loader);
    };

    virtual ~SimulatorScheduler();

    virtual void registered(SchedulerDriver*, const FrameworkID&, const MasterInfo&){
        cout << "Registered!" << endl;
    }

    virtual void reregistered(SchedulerDriver*, const MasterInfo& masterInfo) {}

    virtual void disconnected(SchedulerDriver* driver) {}

    virtual void resourceOffers(SchedulerDriver* driver, const vector<Offer>& offers){
        m_simulator->Run();

        for(size_t i = 0; i < offers.size(); i++){
            LOG(INFO) << "Offers size : " << offers.size();
            const Offer& offer = offers[i];

            double cpus = 0;
            double mem = 0;

            AliSim::SimulatedWallTime simulated_wall_time(20);
            AliSim::AliTraceLoader trace_loader(trace_path);
            AliSim::AliSimulator simulator(simulated_wall_time, trace_loader);
            simulator.Run();

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

            //Launch tasks
            vector<TaskInfo> tasks;

            while(m_task_launched < total_tasks){

            }
        }
    }

private:
    int m_task_launched;
    int m_task_finished;
    int total_tasks;
    AliSim::AliSimulator* m_simulator;
};

int main(int argc, char* argv[]){

    chameleon::set_storage_paths_of_glog("simulator");// provides the program name
    chameleon::set_flags_of_glog();
    AliSim::SimulatedWallTime simulated_wall_time(20);
    AliSim::AliTraceLoader trace_loader(trace_path);
//    multimap<uint64_t, AliSim::Machine_mega> machine_map;
//    traceLoader.Load_machine(machine_map);

    AliSim::AliSimulator simulator(simulated_wall_time, trace_loader);
    simulator.Run();
}

