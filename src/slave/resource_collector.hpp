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
#include <memory>
#include <vector>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// stout dependencies
#include <stout/os.hpp>
#include <stout/tests/utils.hpp>

#include <hardware_resource.pb.h>

#include <cpu_collector.hpp>
#include <disk_collector.hpp>
#include <gpu_collector.hpp>
#include <memory_collector.hpp>
#include <port_collector.hpp>
#include <chameleon_os.hpp>

using std::vector;
using std::string;
using std::shared_ptr;
using std::make_shared;

namespace chameleon {
    class ResourceCollector {
    public:
        explicit ResourceCollector(){
            const Try<os::UTSInfo> info = os::uname();
            CHECK_SOME(info);
            if(info.get().machine == setting::kArmArch){
                msp_cpu = make_shared<ARMCpuCollector>(ARMCpuCollector());
                LOG(INFO)<<"The machine belongs to arm architecture.";
            }else if(info.get().machine == setting::kx86Arch){
                msp_cpu = make_shared<X86CpuCollector>(X86CpuCollector());
                LOG(INFO)<<"The machine belongs to x86 architecture.";
            } else{
                LOG(FATAL)<<"unknown machine architecture, please check!!";
                exit(1);
            }
            msp_disk = make_shared<DiskCollector>(DiskCollector());
            msp_gpu = make_shared<GpuCollector>(GpuCollector());
            msp_mem = make_shared<MemoryCollector>(MemoryCollector());
            msp_port = make_shared<PortCollector>(PortCollector());
        }

        virtual ~ResourceCollector(){

        }

        HardwareResourcesMessage* collect_hardware_resources();

    private:
        shared_ptr<CpuCollector> msp_cpu;
        shared_ptr<DiskCollector> msp_disk;
        shared_ptr<GpuCollector> msp_gpu;
        shared_ptr<MemoryCollector> msp_mem;
        shared_ptr<PortCollector> msp_port;
    };


}


#endif //CHAMELEON_RESOURCECOLLECTOR_HPP
