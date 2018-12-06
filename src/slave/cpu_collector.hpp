/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-3
 * Description：Get Cpu information from computer
 */

#ifndef CHAMELEON_CPU_COLLECTOR_HPP
#define CHAMELEON_CPU_COLLECTOR_HPP

// C++ 11 dependencies
#include <iostream>
#include <fstream>
#include <sstream>
#include <error.h>
#include <string>
#include <vector>

// stout dependencies
#include <stout/option.hpp>
#include <stout/try.hpp>


// libprocess dependencies
#include <process/io.hpp>
#include <process/future.hpp>
#include <process/subprocess.hpp>

#include <hardware_resource.pb.h>


using std::cout;
using std::endl;

using std::stoi;
using std::stod;

using std::vector;
using std::string;

using process::Subprocess;

namespace chameleon {
    /**
     * ClassName   : CpuCollector
     * Date        : 18/11/30
     * Author      : weiguo
     * Description : Collecting CPU information from computer. To satisfy different methods for resources collection for
     * different architectures, we used strategy design patter.
     * */
    class CpuCollector {
    public:
        virtual CPUCollection* get_cpu_info();
    };

    class X86CpuCollector:public CpuCollector{
    public:
        CPUCollection* get_cpu_info();
    };

    class ARMCpuCollector:public CpuCollector {
    public:
        CPUCollection* get_cpu_info();
    };
}

#endif //CHAMELEON_CPU_COLLECTOR_HPP

