//
// Created by lemaker on 18-12-4.
//

#include "runtime_resources_usage.hpp"

#define WAIT_SECOND 1

namespace chameleon {
    /*
    * Function name：get_disk_usage
    * Author       ：heldon
    * Date         ：2018-12-04
    * Description  ：get disk usage
    * Parameter    ：none
    * Return       ：DiskUsage*
    */
    DiskUsage *RuntimeResourceUsage::get_disk_usage() {
        Option<int64_t> disk_available;
        Option<double> available_percent;

        DiskUsage *disk_usage = new DiskUsage();

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
        double decimal = (double) availableDisk / totalsize;
        available_percent = decimal * 100;

        disk_usage->set_disk_available(disk_available.get());
        disk_usage->set_available_percent(available_percent.get());

        return disk_usage;
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
    MemoryUsage *RuntimeResourceUsage::select_memusage() {
        /* message class. */
        MemoryUsage *m_memory_usage = new MemoryUsage();
        std::string::size_type sz;
        string info_string = os::read("/proc/meminfo").get();
        vector<string> m_tokens = strings::tokenize(info_string, "\n");
        //Note that: before Linux kernel 3.14 ,we didn't have the indication named "MemAvailable" in /proc/meminfo
        // Hence, we used the indication "MemFree" to replace "MemAvailable" if the linux kernel version is smaller than 3.14
        bool has_available = false;
        for (int i = 0; i < m_tokens.size(); i++) {
            vector<string> tokens_string = strings::tokenize(m_tokens[i], ":");
            for (auto iter = tokens_string.begin(); iter != tokens_string.end(); iter++) {
                string nospace = strings::trim(*iter);
                if (nospace == "MemTotal") {
                    iter++;
                    m_memory_usage->set_mem_total(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
                if (nospace == "MemFree") {
                    iter++;
                    m_memory_usage->set_mem_free(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
                if (nospace == "MemAvailable") {
                    has_available = true;
                    iter++;
                    m_memory_usage->set_mem_available(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
                if (nospace == "Buffers") {
                    iter++;
                    m_memory_usage->set_buffers(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
                if (nospace == "Cached") {
                    iter++;
                    m_memory_usage->set_cached(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
                if (nospace == "SwapTotal") {
                    iter++;
                    m_memory_usage->set_swap_total(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
                if (nospace == "SwapFree") {
                    iter++;
                    m_memory_usage->set_swap_free(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
                if (nospace == "Hugepagesize") {
                    iter++;
                    m_memory_usage->set_hugepagesize(std::stoi(strings::trim(*iter), &sz));
                    break;
                }
            }
        }

        if(!has_available){
            // we used the indication "MemFree" to replace "MemAvailable" if the linux kernel version is smaller than 3.14
            m_memory_usage->set_mem_available(m_memory_usage->mem_free());
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
    void RuntimeResourceUsage::show_memusage() {
        MemoryUsage *m_memory_usage = select_memusage();
        LOG(INFO) << "MemTotal：" << m_memory_usage->mem_total() << " kB";
        LOG(INFO) << "MemFree：" << m_memory_usage->mem_free() << " kB";
        LOG(INFO) << "MemAvailable：" << m_memory_usage->mem_available() << " kB";
        LOG(INFO) << "Buffers：" << m_memory_usage->buffers() << " kB";
        LOG(INFO) << "Cached：" << m_memory_usage->cached() << " kB";
        LOG(INFO) << "SwapTotal：" << m_memory_usage->swap_total() << " kB";
        LOG(INFO) << "SwapFree：" << m_memory_usage->swap_free() << " kB";
        LOG(INFO) << "Hugepagesize：" << m_memory_usage->hugepagesize() << " kB";
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
        fd = fopen("/proc/stat",
                   "r"); /*Only the first line of 'stat' file and the total information of CPU are read here.*/
        fgets(buff, sizeof(buff), fd);
        sscanf(buff, "%s %u %u %u %u", o->cpu_name, &o->user_time, &o->nice_time, &o->system_time, &o->idle_time);
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
    CPUUsage *RuntimeResourceUsage::cal_cpu_usage(RuntimeResourceUsage::CpuOccupy *first_info,
                                                  RuntimeResourceUsage::CpuOccupy *second_info) {

        double first_time, second_time;
        double user_sub, sys_sub;
        /* cpu class. */
        CPUUsage *m_cpu_usage = new CPUUsage();
        /*The first time (user + nice + system + idle) is assigned to fir_total_time */
        first_time = (double) (first_info->user_time + first_info->nice_time + first_info->system_time +
                               first_info->idle_time);
        /*The second time (user + nice + system + idle) is assigned to sec_total_time */
        second_time = (double) (second_info->user_time + second_info->nice_time + second_info->system_time +
                                second_info->idle_time);
        /*The difference between the first and second time of the user is then assigned to user_sub*/
        user_sub = (double) (second_info->user_time - first_info->user_time);
        /*The difference between the first and second time of the system is then assigned to sys_sub*/
        sys_sub = (double) (second_info->system_time - first_info->system_time);

        /*((user_time+system_time)*100)/(The difference between the first and second total time) , and assigned to m_cpu*/
        auto m_cpu = ((sys_sub + user_sub) * 100.0) / (second_time - first_time);

        m_cpu_usage->set_cpu_used(m_cpu);
        return m_cpu_usage;
    }

    /*
     * Function name：get_net_used_info
     * Author       ：Jessicallo
     * Date         ：2018-12-5
     * Description  ：Read "/proc/net/dev" file and Get the receive usage information of net
     * Parameter    ：NetMessage *net
     * ReturnValue  ：none
     */
    void RuntimeResourceUsage::get_net_used_info(long int *save_rate) {

        char net_card_first = 'e';
        string net_card_name;
        char interface[10];

        /*Output all network card information*/
        Try<set<string>> links = net::links();
        for (auto iter = links.get().begin(); iter != links.get().end(); iter++) {
            net_card_name = *iter;
            if (net_card_name[0] == net_card_first) {
                strcpy(interface, net_card_name.c_str());
            } else {
                continue;
            }
        }

        FILE *net_dev_file;
        char buffer[1024];

        if ((net_dev_file = fopen("/proc/net/dev", "r")) == NULL) {
            printf("open file /proc/net/dev/ error!\n");
            exit(EXIT_FAILURE);
        }

        int i = 0;
        while (i++ < 100) {
            if (fgets(buffer, sizeof(buffer), net_dev_file) != NULL) {
                if (strstr(buffer, interface) != NULL) {
                    sscanf(buffer, "%s %ld", buffer, save_rate);
                    break;
                }
            }
        }
        if (i == 20) *save_rate = 0.01;
        fclose(net_dev_file);
        return;

    }

    NetUsage *RuntimeResourceUsage::cal_net_usage() {
        /* net class */
        NetUsage *m_net_usage = new NetUsage();

        long int start_download_rates;  //Traffic count at the start of saving
        long int end_download_rates;    //Traffic count when saving results
        get_net_used_info(&start_download_rates);
        sleep(WAIT_SECOND);             //How many seconds to sleep, this value is determined according to the value of WAIT_SECOND in the macro definition
        get_net_used_info(&end_download_rates);
        float m_net;
        m_net = (end_download_rates - start_download_rates) / WAIT_SECOND / 1024.00;
        m_net_usage->set_net_used(m_net);
        return m_net_usage;

    }

    RuntimeResourceUsage::RuntimeResourceUsage() {
    }

    RuntimeResourceUsage::~RuntimeResourceUsage() {
    }
}
