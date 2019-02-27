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
            const Try<os::UTSInfo> info = os::uname();
            CHECK_SOME(info);
            if(info.get().machine == setting::kArmArch){
                    LOG(INFO)<<"The machine belongs to arm architecture.";
                    MemoryCollection* memory_collection = new MemoryCollection();
                    const string mem_size = "2048 MB";
                    const string mem_speed="1600 MHz";
                    const string mem_type = "DDR3";
                    memory_collection->set_max_size(mem_size);
                    memory_collection->set_device_quantity(1);
                    MemInfo* mem_info = memory_collection->add_mem_infos();
                    mem_info->set_size(mem_size);
                    mem_info->set_speed(mem_speed);
                    mem_info->set_type(mem_type);
                    hr_message->set_allocated_mem_collection(memory_collection);
            }else {
                    MemoryCollection* memory_collection= msp_mem->select_meminfo();
                    hr_message->set_allocated_mem_collection(memory_collection);
            }


        // disk collector
        DiskCollection* disk_collection = msp_disk->get_disk_collection();
        hr_message->set_allocated_disk_collection(disk_collection);

        // GPU collector
        GPUCollection* hr_gpu = msp_gpu->split_gpu_string();
        hr_message->set_allocated_gpu_collection(hr_gpu);

        // port collector
        PortCollection* hr_port = msp_port->split_port_string();
        hr_message->set_allocated_port_collection(hr_port);

        //tlb collector
        TLBCollection* hr_tlb = msp_tlb->get_tlb_info();
        hr_message->set_allocated_tlb_collection(hr_tlb);
        return hr_message;
    }

}
