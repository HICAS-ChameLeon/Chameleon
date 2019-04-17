//
//* Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
// * Author     ：Lele Li lilelr@163.com
// * Date       ：20119-3-18
//

#ifndef CHAMELEON_CHAMELEON_RESOURCES_HPP
#define CHAMELEON_CHAMELEON_RESOURCES_HPP
// C++ 11 dependencies
#include <iostream>

namespace chameleon {
    class ResourcesOfFramework {
    public:
        explicit ResourcesOfFramework():m_consumped_mem(0),m_consumped_disk(0),m_consumped_cpus(0){

        }
        double m_consumped_cpus;
        double m_consumped_mem;
        uint32_t m_consumped_disk;
    };
}

#endif //CHAMELEON_CHAMELEON_RESOURCES_HPP
