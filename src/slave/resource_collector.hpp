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

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::shared_ptr;
using std::make_shared;

namespace chameleon {
    class ResourceCollector {
    public:
        explicit ResourceCollector(){
//            msp_cpu = make_shared<Cpu_Collector>(Cpu_Collector());
//            msp_disk = make_shared<DiskCollector>(DiskCollector());
//            msp_gpu = make_shared<GpuCollector>(GpuCollector());
//            msp_mem = make_shared<MemoryCollector>(MemoryCollector());
            msp_cpu = new Cpu_Collector();
            msp_disk = new DiskCollector();
            msp_gpu = new GpuCollector();
            msp_mem = new MemoryCollector();

        }

        virtual ~ResourceCollector(){
//            delete msp_cpu;
//            delete msp_disk;
//            delete msp_gpu;
//            delete msp_mem;
        }

        HardwareResourcesMessage collect_hardware_resources(){

            HardwareResourcesMessage hr_message;

            // cpu colletor
            CPUCollection cpu_collection = msp_cpu->get_cpu_info();
            CPUCollection* hr_cpu_collection = new CPUCollection();
//            hr_cpu_collection->set_cpu_quantity(4);
            hr_cpu_collection->CopyFrom(cpu_collection);

            hr_message.set_allocated_cpu_collection(hr_cpu_collection);

            // memeory collector
            msp_mem->get_dmiinfo_rows();
            MemoryCollection* memory_collection= msp_mem->select_meminfo(msp_mem->m_tokens);
            msp_mem->show_meminfo(memory_collection);
            hr_message.set_allocated_mem_collection(memory_collection);

            // disk collector
            DiskCollection* disk_collection = msp_disk->get_disk_collection();
            hr_message.set_allocated_disk_collection(disk_collection);
//            hr_message->set_allocated_disk_collection(nullptr);


            // GPU collector
            string gpu_infos = msp_gpu->get_gpu_string();
            msp_gpu->split_gpu_string(gpu_infos);
//            GPUCollection* gpu_collection = msp_gpu->get_gpu_proto();
//            gpu_collection->set_gpu_quantity(1);
//            hr_message->set_allocated_gpu_collection(gpu_collection);

            GPUCollection* t_gpu = new GPUCollection();
            t_gpu->set_gpu_quantity(1);
            hr_message.set_allocated_gpu_collection(t_gpu);

//            int a = 4;
            return hr_message;
        }

    private:
//        shared_ptr<Cpu_Collector> msp_cpu;
//        shared_ptr<DiskCollector> msp_disk;
//        shared_ptr<GpuCollector> msp_gpu;
//        shared_ptr<MemoryCollector> msp_mem;
        Cpu_Collector* msp_cpu;
        DiskCollector* msp_disk;
        GpuCollector* msp_gpu;
        MemoryCollector* msp_mem;
    };


}


#endif //CHAMELEON_RESOURCECOLLECTOR_HPP
