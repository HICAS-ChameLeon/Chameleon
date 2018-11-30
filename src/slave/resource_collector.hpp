//
// Created by lemaker on 18-11-30.
//

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
