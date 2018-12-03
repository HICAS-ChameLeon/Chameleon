/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：marcie 316187187@qq.com
 * Date       ：18-11-30
 * Description：Get memory information
 */
#ifndef LIBPROCESS_START_DMIMEMINFO_H
#define LIBPROCESS_START_DMIMEMINFO_H

// C++ 11 dependencies
#include <iostream>

// stout dependencies
#include <stout/strings.hpp>

// libprocess dependencies
#include <process/subprocess.hpp>
#include <process/io.hpp>

// chameleon headers
#include <hardware_resource.pb.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;

using process::Subprocess;
using process::Future;

namespace chameleon {
    /*
     * Class name ：MemoryCollector
     * Author     ：marcie  316187187@qq.com
     * Date       ：18-11-30
     * Description：A Class to get memory information
     */
    class MemoryCollector {
    private:
        /* storing strings during processing. */
        vector<string> m_tokens;

        /* message class. */
        MemoryCollection* m_memory_collection = new MemoryCollection();
    public:
        /*
         * Function name：get_dmiinfo_rows
         * Author       ：marcie
         * Date         ：2018-11-30
         * Description  ：Input command and get the returned information
         * Parameter    ：none
         * Return       ：vector<string> m_tokens
         */
        vector<string> get_dmiinfo_rows() {
            /* amount to input command and get the returned memory information. */
            Try<Subprocess> s = subprocess(
                    "sudo -S dmidecode -t memory",
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
        MemoryCollection* select_meminfo() {
            get_dmiinfo_rows();
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
         * Function name：show_meminfo
         * Author       ：marcie
         * Date         ：2018-11-30
         * Description  ：output dmidecode infomation by protobuf message
         * Parameter    ：none
         * Output       :memory information
         * Return       ：none
         */
        void show_meminfo() {
            select_meminfo();
            cout << "Maximum Capacity：" << m_memory_collection->max_size() << endl;
            /* memoryCollection.info().size() */
            cout << "当前机器有" << m_memory_collection->device_quantity() << "个使用中的内存插槽" << endl;
            for (auto iter = m_memory_collection->mem_infos().begin(); iter != m_memory_collection->mem_infos().end(); iter++) {
                cout << "第" << iter - m_memory_collection->mem_infos().begin() + 1 << "个Memory Device的信息为：" << endl;
                cout << "Size：" << iter.operator->()->size() << endl;
                cout << "Type：" << iter.operator->()->type() << endl;
                cout << "Speed：" << iter.operator->()->speed() << endl;
            }
        }

        MemoryCollector() {
        }

        ~MemoryCollector() {
        }
    };
}
#endif //LIBPROCESS_START_DMIMEMINFO_H