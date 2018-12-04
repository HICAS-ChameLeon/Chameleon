//
// Created by lemaker on 18-12-4.
//

#ifndef CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP
#define CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP

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
        MemoryUsage* m_memory_usage;
    public:
        /*
         * Function name：select_memusage
         * Author       ：marcie
         * Date         ：2018-12-4
         * Description  ：Input command and get the returned information,
         *                divide strings and filter out needed information
         * Parameter    ：none
         * Return       ：MemoryUsage m_memory_usage
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

        RuntimeResourceUsage();

        ~RuntimeResourceUsage();
    };
}


#endif //CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP
