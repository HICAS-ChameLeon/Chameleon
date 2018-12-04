//
// Created by lemaker on 18-12-4.
//

#ifndef CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP
#define CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP

//C++11 dependencies
#include <iostream>
#include <sys/statfs.h>

//stout dependencies
#include <stout/option.hpp>
#include <stout/stringify.hpp>

//chameleon headers
#include <runtime_resource.pb.h>

using std::string;

namespace  chameleon{

    class RuntimeResourceUsage {
    public:
        /*
        * Function name：get_disk_usage
        * Author       ：heldon
        * Date         ：2018-12-04
        * Description  ：get disk usage
        * Parameter    ：none
        * Return       ：DiskUsage*
        */
        DiskUsage* get_disk_usage();
    };
}



#endif //CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP
