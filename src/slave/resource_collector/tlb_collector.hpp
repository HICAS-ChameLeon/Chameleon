/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-4
 * Description：Get TLB information from computer
 */

#ifndef CHAMELEON_TLB_COLLECTOR_HPP
#define CHAMELEON_TLB_COLLECTOR_HPP

#endif //CHAMELEON_TLB_COLLECTOR_HPP

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
using std::vector;
using std::string;

using process::Subprocess;

namespace chameleon{
    /**
     * ClassName   :TLBCollector
     * Date        :18/12/4
     * Author      :weiguow
     * Description :Collecting TLB information from computer
     * */
    class TLBCollector {
    public:
        TLBCollection* get_tlb_info();
    };
}

