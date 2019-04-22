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
        trace_loader->Load_task(&m_tasks_map);
    }

    multimap<uint64_t, Machine_mega> SimulatorLinker::Get_machine_mega() {
        multimap<uint64_t, Machine_mega>::iterator it = m_machine_mega_map.begin();
        while(it != m_machine_mega_map.end()){
            cout << it->first << " -> " << endl;

            auto count = m_machine_mega_map.count(it->first);
            for(auto i = 0; i != count; ++it, ++ i){
                cout << it->second << endl;
            }
        }
        return m_machine_mega_map;
    }

    void SimulatorLinker::Get_tasks() {
        multimap<uint64_t, Task_identifier>::iterator it = m_tasks_map.begin();
        while (it != m_tasks_map.end()){
            cout << it->first << "->" << endl;

            auto count = m_tasks_map.count(it->first);
            for(auto i = 0; i != count; i++, it++){
                cout << it->second << endl;
            }
        }
    }
}