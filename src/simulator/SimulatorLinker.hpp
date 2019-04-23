//
// Created by Heldon on 19-4-15.
//

#ifndef CHAMELEON_SIMULATORLINKER_HPP
#define CHAMELEON_SIMULATORLINKER_HPP

#include <map>
#include "SimulatedWallTime.hpp"
#include "AliTracePreProcessor.h"
#include "AliTraceLoader.hpp"

namespace AliSim{
    class SimulatorLinker {
    private:
        SimulatedWallTime* m_simulated_time;
        multimap<uint64_t, Machine_mega> m_machine_mega_map;
        multimap<uint64_t, Task_identifier> m_tasks_map;

        multimap<uint64_t, Task_identifier> m_current_tasks_map;

    public:
        explicit SimulatorLinker(SimulatedWallTime* simulated_time);

        ~SimulatorLinker();

        void Load_trace_data(AliTraceLoader* trace_loader);

        multimap<uint64_t, Machine_mega> Get_machine_mega();

        multimap<uint64_t, Task_identifier> Get_tasks();
    };
}




#endif //CHAMELEON_SIMULATORLINKER_HPP
