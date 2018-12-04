//
// Created by lemaker on 18-12-4.
//

#include "runtime_resources_usage.hpp"

namespace chameleon{
    /*
     * Function name：select_meminfo
     * Author       ：marcie
     * Date         ：2018-11-30
     * Description  ：Input command and get the returned information,
     *                divide strings and filter out needed information
     * Parameter    ：none
     * Return       ：MemoryCollection m_memory_collection
     */
    MemoryUsage* chameleon::RuntimeResourceUsage::select_memusage() {
        /* amount to input command and get the returned memory information. */
        Try<Subprocess> s = subprocess(
                "cat /proc/meminfo",
                Subprocess::FD(STDIN_FILENO),
                Subprocess::PIPE(),
                Subprocess::FD(STDERR_FILENO));
        Future<string> info = process::io::read(s.get().out().get());
        /* convert format to string. */
        string info_string = info.get();
        /* divide strings by "\n", that is divide it into rows. */
        vector<string> m_tokens = strings::tokenize(info_string, "\n");
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

    void RuntimeResourceUsage::get_cpu_used_info(RuntimeResourceUsage::CpuOccupy *o) {
        FILE *fd;
        char buff[MAXBUFSIZE];
        fd = fopen ("/proc/stat", "r"); //这里只读取stat文件的第一行及cpu总信息，如需获取每核cpu的使用情况，请分析stat文件的接下来几行。
        fgets (buff, sizeof(buff), fd);
        sscanf (buff, "%s %u %u %u %u", o->cpu_name, &o->user_time, &o->nice_time,&o->system_time, &o->idle_time);
        printf(buff);
        fclose(fd);
    }

    CPUUsage* RuntimeResourceUsage::cal_cpu_usage(RuntimeResourceUsage::CpuOccupy *first_info,
                                             RuntimeResourceUsage::CpuOccupy *second_info) {

        m_cpu_usage = new CPUUsage();

        double fir_total_time, sec_total_time;
        double user_sub, sys_sub;

        fir_total_time = (double) (first_info->user_time + first_info->nice_time + first_info->system_time +first_info->idle_time);//第一次(用户+优先级+系统+空闲)的时间再赋给od
        sec_total_time = (double) (second_info->user_time + second_info->nice_time + second_info->system_time +second_info->idle_time);//第二次(用户+优先级+系统+空闲)的时间再赋给od

        user_sub = (double) (second_info->user_time - first_info->user_time);    //用户第一次和第二次的时间之差再赋给id
        sys_sub = (double) (second_info->system_time - first_info->system_time);//系统第一次和第二次的时间之差再赋给sd

        float m_cpu;
        m_cpu = ((sys_sub+user_sub)*100.0)/(sec_total_time-fir_total_time); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used

        m_cpu_usage->set_cpu_used(m_cpu);

        return m_cpu_usage;
    }
}