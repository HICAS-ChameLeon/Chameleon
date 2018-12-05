//
// Created by lemaker on 18-12-4.
//

#include <runtime_resources_usage.hpp>
namespace chameleon{
    /*
    * Function name：get_disk_usage
    * Author       ：heldon
    * Date         ：2018-12-04
    * Description  ：get disk usage
    * Parameter    ：none
    * Return       ：DiskUsage*
    */
    DiskUsage* chameleon::RuntimeResourceUsage::get_disk_usage(){
        Option<int64_t> disk_available;
        Option<double > available_percent;

        DiskUsage* disk_usage;

        /*Use statfs to get disk's rest storage,available storage*/
        struct statfs diskInfo;
        /*Find the information under the path "/home" */
        statfs("/home", &diskInfo);

        /*The number of bytes contained in each block*/
        unsigned long long blocksize = diskInfo.f_bsize;
        /*The total number of bytes, f_blocks is the number of blocks*/
        unsigned long long totalsize = blocksize * diskInfo.f_blocks;
        /*The size of the avaliable space*/
        unsigned long long availableDisk = diskInfo.f_bavail * blocksize;

        disk_available = availableDisk;
        double decimal = (double)availableDisk/totalsize;
        available_percent = decimal * 100;

        disk_usage->set_disk_available(disk_available.get());
        disk_usage->set_available_percent(available_percent.get());

        return  disk_usage;
    }

}
  
    /*
     * Function name：select_memusage
     * Author       ：marcie
     * Date         ：2018-12-4
     * Description  ：Input command and get the returned information,
     *                divide strings and filter out needed information
     * Parameter    ：none
     * Return       ：MemoryUsage m_memory_usage
     */
    MemoryUsage* chameleon::RuntimeResourceUsage::select_memusage() {
        string info_string = os::read("/proc/meminfo").get();
        vector<string> m_tokens = strings::tokenize(info_string, "\n");
        m_memory_usage = new MemoryUsage();
        for (int i = 0; i < m_tokens.size(); i++) {
            vector<string> tokens_string = strings::tokenize(m_tokens[i],":");
            for (auto iter = tokens_string.begin(); iter != tokens_string.end(); iter++) {
                string nospace = strings::trim(*iter);
                if(nospace == "MemTotal"){
                    iter++;
                    m_memory_usage->set_mem_total(strings::trim(*iter));
                    break;
                }
                if(nospace == "MemFree"){
                    iter++;
                    m_memory_usage->set_mem_free(strings::trim(*iter));
                    break;
                }
                if(nospace == "MemAvailable"){
                    iter++;
                    m_memory_usage->set_mem_available(strings::trim(*iter));
                    break;
                }
                if(nospace == "Buffers"){
                    iter++;
                    m_memory_usage->set_buffers(strings::trim(*iter));
                    break;
                }
                if(nospace == "Cached"){
                    iter++;
                    m_memory_usage->set_cached(strings::trim(*iter));
                    break;
                }
                if(nospace == "SwapTotal"){
                    iter++;
                    m_memory_usage->set_swap_total(strings::trim(*iter));
                    break;
                }
                if(nospace == "SwapFree"){
                    iter++;
                    m_memory_usage->set_swap_free(strings::trim(*iter));
                    break;
                }
                if(nospace == "Hugepagesize"){
                    iter++;
                    m_memory_usage->set_hugepagesize(strings::trim(*iter));
                    break;
                }
            }
        }
        return m_memory_usage;
    }

    /*
     * Function name：show_memusage
     * Author       ：marcie
     * Date         ：2018-12-4
     * Description  ：output memory usage infomation by protobuf message
     * Parameter    ：none
     * Output       :memory usage information
     * Return       ：none
     */
    void chameleon::RuntimeResourceUsage::show_memusage() {
        select_memusage();
        LOG(INFO) << "MemTotal：" << m_memory_usage->mem_total();
        LOG(INFO) << "MemFree：" << m_memory_usage->mem_free();
        LOG(INFO) << "MemAvailable：" << m_memory_usage->mem_available();
        LOG(INFO) << "Buffers：" << m_memory_usage->buffers();
        LOG(INFO) << "Cached：" << m_memory_usage->cached();
        LOG(INFO) << "SwapTotal：" << m_memory_usage->swap_total();
        LOG(INFO) << "SwapFree：" << m_memory_usage->swap_free();
        LOG(INFO) << "Hugepagesize：" << m_memory_usage->hugepagesize();
    }


    /*
     * Function name：get_cpu_used_info
     * Author       ：zhangyixin
     * Date         ：2018-12-4
     * Description  ：Read "/proc/stat" file and Get the total information of CPU usage
     * Parameter    ：CpuOccupy *o
     * ReturnValue  ：none
     */
    void RuntimeResourceUsage::get_cpu_used_info(RuntimeResourceUsage::CpuOccupy *o) {
        FILE *fd;
        char buff[MAXBUFSIZE];
        fd = fopen ("/proc/stat", "r"); /*Only the first line of 'stat' file and the total information of CPU are read here.*/
        fgets (buff, sizeof(buff), fd);
        sscanf (buff, "%s %u %u %u %u", o->cpu_name, &o->user_time, &o->nice_time,&o->system_time, &o->idle_time);
        /*printf(buff);*/
        fclose(fd);
    }

    /*
     * Function name：cal_cpu_usage
     * Author       ：zhangyixin
     * Date         ：2018-12-4
     * Description  ：Calculate CPU usage , and Save the results to protobuf
     * Parameter    ：first_info,second_info
     * ReturnValue  ：CPUUsage*
     */
    CPUUsage* RuntimeResourceUsage::cal_cpu_usage(RuntimeResourceUsage::CpuOccupy *first_info,
                                             RuntimeResourceUsage::CpuOccupy *second_info) {
        m_cpu_usage = new CPUUsage() ;
        double fir_total_time, sec_total_time;
        double user_sub, sys_sub;

        /*The first time (user + nice + system + idle) is assigned to fir_total_time */
        fir_total_time = (double) (first_info->user_time + first_info->nice_time + first_info->system_time +first_info->idle_time);
        /*The second time (user + nice + system + idle) is assigned to sec_total_time */
        sec_total_time = (double) (second_info->user_time + second_info->nice_time + second_info->system_time +second_info->idle_time);
        /*The difference between the first and second time of the user is then assigned to user_sub*/
        user_sub = (double) (second_info->user_time - first_info->user_time);
        /*The difference between the first and second time of the system is then assigned to sys_sub*/
        sys_sub = (double) (second_info->system_time - first_info->system_time);

        float m_cpu;
        /*((user_time+system_time)*100)/(The difference between the first and second total time) , and assigned to m_cpu*/
        m_cpu = ((sys_sub+user_sub)*100.0)/(sec_total_time-fir_total_time);

        m_cpu_usage->set_cpu_used(m_cpu);
        return m_cpu_usage;
    }

    chameleon::RuntimeResourceUsage::RuntimeResourceUsage() {
    }

    chameleon::RuntimeResourceUsage::~RuntimeResourceUsage() {
    }
}
