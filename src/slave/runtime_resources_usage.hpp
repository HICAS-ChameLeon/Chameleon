//
// Created by lemaker on 18-12-4.
//

#ifndef CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP
#define CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP

#define MAXBUFSIZE 1024

// C++ 11 dependencies
#include <iostream>

// stout dependencies
#include <stout/strings.hpp>

// libprocess dependencies
#include <process/subprocess.hpp>
#include <process/io.hpp>

// chameleon headers
#include <runtime_resource.pb.h>

using std::string;
using std::vector;

using process::Subprocess;
using process::Future;

namespace chameleon {
    class RuntimeResourceUsage {
    private:
        /* message class. */
        MemoryUsage* m_memory_usage = new MemoryUsage();

        /* cpu class. */
        CPUUsage* m_cpu_usage;
        typedef struct CPUTime
        {
            char cpu_name[20];
            unsigned int user_time;
            unsigned int nice_time;
            unsigned int system_time;
            unsigned int idle_time;
        } CpuOccupy ;

    public:
        /*
         * Function name：select_meminfo
         * Author       ：marcie
         * Date         ：2018-11-30
         * Description  ：Input command and get the returned information,
         *                divide strings and filter out needed information
         * Parameter    ：none
         * Return       ：MemoryCollection m_memory_collection
         */
        MemoryUsage* select_memusage();

        /*
         * Function name：show_memusage
         * Author       ：marcie
         * Date         ：2018-12-4
         * Description  ：output memory usage infomation by protobuf message
         * Parameter    ：none
         * Output       :memory usage information
         * Return       ：none
         */
        void show_memusage();

        /* cpu function */
        void get_cpu_used_info (CpuOccupy *o) ;
        CPUUsage* cal_cpu_usage (CpuOccupy *first_info, CpuOccupy *second_info) ;
    };
}


#endif //CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP