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

#include <hardware_resource.pb.h>

#include <cpu_collector.hpp>
#include <disk_collector.hpp>
#include <gpu_collector.hpp>
#include <memory_collector.hpp>
#include <port_collector.hpp>

using std::vector;
using std::string;
using std::shared_ptr;
using std::make_shared;

namespace chameleon {
    class ResourceCollector {
    public:
        explicit ResourceCollector(){
            msp_cpu = make_shared<CpuCollector>(CpuCollector());
            msp_disk = make_shared<DiskCollector>(DiskCollector());
            msp_gpu = make_shared<GpuCollector>(GpuCollector());
            msp_mem = make_shared<MemoryCollector>(MemoryCollector());
            msp_port = make_shared<PortCollector>(PortCollector());
        }

        virtual ~ResourceCollector(){

        }

        HardwareResourcesMessage* collect_hardware_resources(){

            HardwareResourcesMessage* hr_message=new HardwareResourcesMessage();

            // cpu colletor
            auto hr_cpu_collection = msp_cpu->get_cpu_info();
            hr_message->set_allocated_cpu_collection(hr_cpu_collection);

            // memeory collector
            MemoryCollection* memory_collection= msp_mem->select_meminfo();
            hr_message->set_allocated_mem_collection(memory_collection);

            // disk collector
            DiskCollection* disk_collection = msp_disk->get_disk_collection();
            hr_message->set_allocated_disk_collection(disk_collection);

            // GPU collector
            GPUCollection* hr_gpu = msp_gpu->split_gpu_string();
            hr_message->set_allocated_gpu_collection(hr_gpu);

            PortCollection* hr_port = msp_port->split_port_string();
            hr_message->set_allocated_port_collection(hr_port);
            return hr_message;
        }

    private:
        shared_ptr<CpuCollector> msp_cpu;
        shared_ptr<DiskCollector> msp_disk;
        shared_ptr<GpuCollector> msp_gpu;
        shared_ptr<MemoryCollector> msp_mem;
        shared_ptr<PortCollector> msp_port;
    };


}


#endif //CHAMELEON_RESOURCECOLLECTOR_HPP
