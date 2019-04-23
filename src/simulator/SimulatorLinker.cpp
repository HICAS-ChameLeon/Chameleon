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
        return m_machine_mega_map;
    }

    multimap<uint64_t, Task_identifier> SimulatorLinker::Get_tasks() {
        return m_tasks_map;
    }
}