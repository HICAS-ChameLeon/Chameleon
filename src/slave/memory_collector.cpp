/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：marcie 316187187@qq.com
 * Date       ：18-12-4
 * Description：Get memory information
 */

#include <memory_collector.hpp>

namespace chameleon{
    /*
     * Function name：get_dmiinfo_rows
     * Author       ：marcie
     * Date         ：2018-11-30
     * Description  ：Input command and get the returned information
     * Parameter    ：none
     * Return       ：vector<string> m_tokens
     */

    vector<string> chameleon::MemoryCollector::get_info_rows(string string1) {
        /* amount to input command and get the returned memory information. */
        Try<Subprocess> s = subprocess(
                string1,
                Subprocess::FD(STDIN_FILENO),
                Subprocess::PIPE(),
                Subprocess::FD(STDERR_FILENO));
        Future<string> info = process::io::read(s.get().out().get());
        /* convert format to string. */
        string info_string = info.get();
        /* divide strings by "\n", that is divide it into rows. */
        m_tokens = strings::tokenize(info_string, "\n");
        return m_tokens;
    }

    /*
     * Function name：select_meminfo
     * Author       ：marcie
     * Date         ：2018-11-30
     * Description  ：Input command and get the returned information,
     *                divide strings and filter out needed information
     * Parameter    ：none
     * Return       ：MemoryCollection m_memory_collection
     */
    MemoryCollection* chameleon::MemoryCollector::select_meminfo() {
        get_info_rows("sudo -S dmidecode -t memory");
        /* the number of size,type,speed. */
        int num_size = 0, num_type = 0, num_speed = 0;
        /* class ptr. */
        MemInfo *tmp;
        /* divide strings by ":", and filter memory information. */
        for (int i = 0; i < m_tokens.size(); i++) {
            vector<string> tokens_string = strings::tokenize(m_tokens[i], ":");
            for (auto iter = tokens_string.begin(); iter != tokens_string.end(); iter++) {
                string nospace = strings::trim(*iter);
                if (nospace == "Maximum Capacity") {
                    iter++;
                    m_memory_collection->set_max_size(strings::trim(*iter));
                }
                if (nospace == "Size") {
                    iter++;
                    if (strings::trim(*iter) != "No Module Installed") {
                        tmp = m_memory_collection->add_mem_infos();
                        tmp->set_size(strings::trim(*iter));
                        num_size++;
                    }
                    iter--;
                }
                if (strings::trim(*iter) == "Type") {
                    iter++;
                    if (num_size > num_type) {
                        tmp->set_type(strings::trim(*iter));
                        num_type++;
                    }
                    iter--;
                }
                if (strings::trim(*iter) == "Speed") {
                    iter++;
                    if (strings::trim(*iter) != "Unknown") {
                        tmp->set_speed(strings::trim(*iter));
                        num_speed++;
                    }
                    iter--;
                }
            }
        }
        m_memory_collection->set_device_quantity(num_size);
        return m_memory_collection;
    }

    /*
     * Function name：select_meminfo
     * Author       ：marcie
     * Date         ：2018-12-4
     * Description  ：Input command and get the returned information,
     *                divide strings and filter out needed information
     * Parameter    ：none
     * Return       ：MemoryUsage m_memory_usage
     */
    MemoryUsage* chameleon::MemoryCollector::select_memusage() {
        get_info_rows("cat /proc/meminfo");
        for (int i = 0; i < m_tokens.size(); i++) {
            vector<string> tokens_string = strings::tokenize(m_tokens[i],":");
            for (auto iter = tokens_string.begin(); iter != tokens_string.end(); iter++) {
                string nospace = strings::trim(*iter);
                if(nospace == "MemTotal"){
                    iter++;
                    m_memory_usage.set_mem_total(strings::trim(*iter));
                }
                if(nospace == "MemFree"){
                    iter++;
                    m_memory_usage.set_mem_free(strings::trim(*iter));
                }
                if(nospace == "MemAvailable"){
                    iter++;
                    m_memory_usage.set_mem_available(strings::trim(*iter));
                }
                if(nospace == "Buffers"){
                    iter++;
                    m_memory_usage.set_buffers(strings::trim(*iter));
                }
                if(nospace == "Cached"){
                    iter++;
                    m_memory_usage.set_cached(strings::trim(*iter));
                }
                if(nospace == "SwapTotal"){
                    iter++;
                    m_memory_usage.set_swap_total(strings::trim(*iter));
                }
                if(nospace == "SwapFree"){
                    iter++;
                    m_memory_usage.set_swap_free(strings::trim(*iter));
                }
                if(nospace == "Hugepagesize"){
                    iter++;
                    m_memory_usage.set_hugepagesize(strings::trim(*iter));
                }
            }
        }
        return m_memory_usage;
    }

    /*
     * Function name：show_meminfo
     * Author       ：marcie
     * Date         ：2018-11-30
     * Description  ：output dmidecode infomation by protobuf message
     * Parameter    ：none
     * Output       :memory information
     * Return       ：none
     */
    void chameleon::MemoryCollector::show_meminfo() {
        select_meminfo();
        LOG(INFO) << "Maximum Capacity：" << m_memory_collection->max_size();
        /* memoryCollection.info().size() */
        LOG(INFO) << "当前机器有" << m_memory_collection->device_quantity() << "个使用中的内存插槽";
        for (auto iter = m_memory_collection->mem_infos().begin(); iter != m_memory_collection->mem_infos().end(); iter++) {
            LOG(INFO) << "第" << iter - m_memory_collection->mem_infos().begin() + 1 << "个Memory Device的信息为：";
            LOG(INFO) << "Size：" << iter.operator->()->size();
            LOG(INFO) << "Type：" << iter.operator->()->type();
            LOG(INFO) << "Speed：" << iter.operator->()->speed();
        }
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
    void chameleon::MemoryCollector::show_memusage() {
        select_memusage();
        LOG(INFO) << "MemTotal：" << m_memory_usage.mem_total;
        LOG(INFO) << "MemFree：" << m_memory_usage.mem_free;
        LOG(INFO) << "MemAvailable：" << m_memory_usage.mem_available;
        LOG(INFO) << "Buffers：" << m_memory_usage.buffers;
        LOG(INFO) << "Cached：" << m_memory_usage.cached;
        LOG(INFO) << "SwapTotal：" << m_memory_usage.swap_total;
        LOG(INFO) << "SwapFree：" << m_memory_usage.swap_free;
        LOG(INFO) << "Hugepagesize：" << m_memory_usage.hugepagesize;
    }

    chameleon::MemoryCollector::MemoryCollector() {
    }

    chameleon::MemoryCollector::~MemoryCollector() {
    }
}