//
// Created by lemaker on 18-12-4.
//

#include "runtime_resources_usage.hpp"

namespace chameleon{
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

    chameleon::RuntimeResourceUsage::RuntimeResourceUsage() {
    }

    chameleon::RuntimeResourceUsage::~RuntimeResourceUsage() {
    }
}