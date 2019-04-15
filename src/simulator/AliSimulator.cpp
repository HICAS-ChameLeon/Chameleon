//
// Created by Heldon on 19-4-15.
//

#include "AliSimulator.hpp"

namespace AliSim{
    AliSimulator::AliSimulator(SimulatedWallTime simulated_wall_time, AliTraceLoader ali_trace_loader)
        : m_simulated_wall_time(simulated_wall_time), m_ali_trace_loader(ali_trace_loader){
        m_linker = new SimulatorLinker(&m_simulated_wall_time);
    }

    AliSimulator::~AliSimulator() {
        delete m_linker;
        cout << "delete m_linker" << endl;
    }

    void AliSimulator::Run() {
        ReplaySimulation();
    }

    void AliSimulator::ReplaySimulation() {
        m_linker->Load_trace_data(&m_ali_trace_loader);

        //开始模拟
        //int cnt = 1;
        while (m_simulated_wall_time.Get_current_time_stamp() < 10){
            m_simulated_wall_time.Increase_time_stamp_by_one_second();

        }
        m_linker->Get_machine_mega();
    }
}