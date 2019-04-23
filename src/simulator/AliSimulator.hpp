//
// Created by Heldon on 19-4-15.
//

#ifndef CHAMELEON_ALISIMULATOR_HPP
#define CHAMELEON_ALISIMULATOR_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "AliTraceLoader.hpp"
#include "SimulatorLinker.hpp"

namespace AliSim{
    class AliSimulator {
    private:
        SimulatorLinker* m_linker;
        SimulatedWallTime m_simulated_wall_time;
        AliTraceLoader m_ali_trace_loader;

        void ReplaySimulation();
    public:
        explicit AliSimulator(SimulatedWallTime simulated_wall_time,AliTraceLoader ali_trace_loader);

        ~AliSimulator();

        void Run();

        multimap<uint64_t, Machine_mega> Get_machine();
        multimap<uint64_t, Task_identifier> Get_task();
    };
}



#endif //CHAMELEON_ALISIMULATOR_HPP
