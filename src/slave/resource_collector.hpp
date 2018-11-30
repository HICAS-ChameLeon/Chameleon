/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-30
 * Description：ResourceCollector
 */
#ifndef CHAMELEON_RESOURCECOLLECTOR_HPP
#define CHAMELEON_RESOURCECOLLECTOR_HPP

// C++ 11 dependencies
#include <iostream>

#include <hardware_resource.pb.h>

namespace chameleon {
    class ResourceCollector {
    public:
        explicit ResourceCollector(){

        }

        virtual ~ResourceCollector(){

        }

        HardwareResourcesMessage& collect_hardware_resources(){

        }
    };


}


#endif //CHAMELEON_RESOURCECOLLECTOR_HPP
