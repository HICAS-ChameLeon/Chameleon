/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-30
 * Description：ResourceCollector
 */
#include "resource_collector.hpp"
namespace chameleon {
    HardwareResourcesMessage* ResourceCollector::collect_hardware_resources(){

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

}
