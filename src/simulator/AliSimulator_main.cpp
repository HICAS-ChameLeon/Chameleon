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

class SimulatorScheduler : public Scheduler{
public:
    //construct function
    SimulatorScheduler();

    virtual ~SimulatorScheduler();

    virtual void registered(SchedulerDriver*, const FrameworkID&, const MasterInfo&){
        cout << "Registered!" << endl;
    }

    virtual void reregistered(SchedulerDriver*, const MasterInfo& masterInfo) {}

    virtual void disconnected(SchedulerDriver* driver) {}

    virtual void resourceOffers(SchedulerDriver* driver, const vector<Offer>& offers){

        for(size_t i = 0; i < offers.size(); i++){
            LOG(INFO) << "Offers size : " << offers.size();
            const Offer& offer = offers[i];

            double cpus = 0;
            double mem = 0;

            for(int i = 0; i < offer.resources_size(); i++){

            }
        }
    }
};


const string trace_path = "/home/heldon/chameleon/ali_clusterdata/alibaba_clusterdata_v2018/";

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

