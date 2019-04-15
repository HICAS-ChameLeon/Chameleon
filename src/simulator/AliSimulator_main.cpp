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

#include <mesos.pb.h>

#include "AliTracePreProcessor.h"
#include "AliTraceLoader.hpp"
#include "AliSimulator.hpp"

using namespace mesos;

class SimulatorScheduler : public Scheduler{

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

