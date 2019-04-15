//
// Created by root on 19-4-15.
//

#ifndef CHAMELEON_SIMULATEDWALLTIME_HPP
#define CHAMELEON_SIMULATEDWALLTIME_HPP

#include <cstdint>

namespace AliSim{
    class SimulatedWallTime {
    private:
        uint64_t m_current_time_stamp;  //当前时间戳
        uint64_t m_stop_time_stamp;  //结束时间戳

    public:
        explicit SimulatedWallTime(uint64_t stop_time_stamp);  //构造函数 参数： 结束时间 stop_time_stamp

        uint64_t Get_current_time_stamp();

        uint64_t Increase_time_stamp_by_one_second();
    };
}



#endif //CHAMELEON_SIMULATEDWALLTIME_HPP
