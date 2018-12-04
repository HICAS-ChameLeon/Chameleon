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
#include <runtime_resource.pb.h>

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
        vector<string> get_info_rows(string string1);

        /*
         * Function name：select_meminfo
         * Author       ：marcie
         * Date         ：2018-11-30
         * Description  ：Input command and get the returned information,
         *                divide strings and filter out needed information
         * Parameter    ：none
         * Return       ：MemoryCollection m_memory_collection
         */
        MemoryCollection* select_meminfo();

        /*
         * Function name：show_meminfo
         * Author       ：marcie
         * Date         ：2018-11-30
         * Description  ：output dmidecode infomation by protobuf message
         * Parameter    ：none
         * Output       :memory information
         * Return       ：none
         */
        void show_meminfo();

        MemoryCollector();

        ~MemoryCollector();
    };
}
#endif //LIBPROCESS_START_DMIMEMINFO_H