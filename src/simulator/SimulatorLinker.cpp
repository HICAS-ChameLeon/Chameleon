//
// Created by Heldon on 19-4-15.
//

#include <map>
#include "SimulatorLinker.hpp"

namespace AliSim{
    SimulatorLinker::SimulatorLinker(AliSim::SimulatedWallTime *simulated_time) {
        m_simulated_time = simulated_time;
    }

    SimulatorLinker::~SimulatorLinker() {
        //delete m_simulated_time;
    }

    void SimulatorLinker::Load_trace_data(AliSim::AliTraceLoader *trace_loader) {
        trace_loader->Load_machine(&m_machine_mega_map);
    }

    void SimulatorLinker::Get_machine_mega() {
        multimap<uint64_t, Machine_mega>::iterator it = m_machine_mega_map.begin();
        while(it != m_machine_mega_map.end()){
            cout << it->first << " -> " << endl;

            auto count = m_machine_mega_map.count(it->first);
            for(auto i = 0; i != count; ++it, ++ i){
                cout << it->second << endl;
            }
        }
    }
}