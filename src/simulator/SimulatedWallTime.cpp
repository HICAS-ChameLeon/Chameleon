//
// Created by Heldon on 19-4-15.
//

#include "SimulatedWallTime.hpp"

namespace AliSim{
    SimulatedWallTime::SimulatedWallTime(uint64_t stop_time_stamp) {
        this->m_current_time_stamp = 0;
        this->m_stop_time_stamp = stop_time_stamp;
    }

    uint64_t SimulatedWallTime::Get_current_time_stamp() {
            return m_current_time_stamp;
    }

    uint64_t SimulatedWallTime::Increase_time_stamp_by_one_second() {
        m_current_time_stamp++;
        return m_current_time_stamp;
    }
}
