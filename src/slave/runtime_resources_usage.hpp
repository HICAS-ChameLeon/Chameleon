/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li,Kun ma,Yixing zhang, Hongtai zhang, Weiguo Wang lilelr@163.com
 * Date       ：18-12-4
 * Description：runtime resources usage
 */

#ifndef CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP
#define CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP

//C++11 dependencies
#define MAXBUFSIZE 1024

#include <iostream>
#include <sys/statfs.h>
#include <vector>

//stout dependencies
#include <stout/os.hpp>
#include <stout/option.hpp>
#include <stout/stringify.hpp>
#include <stout/try.hpp>
#include <stout/net.hpp>

//chameleon headers
#include <runtime_resource.pb.h>

using std::string;
using std::vector;
using std::set;
using net::links;

namespace  chameleon{

    class RuntimeResourceUsage {

    public:


        /*
        * Function name：get_disk_usage
        * Author       ：heldon
        * Date         ：2018-12-04
        * Description  ：get disk usage
        * Parameter    ：none
        * Return       ：DiskUsage*
        */
        DiskUsage* get_disk_usage();

        /*
         * Function name：select_memusage
         * Author       ：marcie
         * Date         ：2018-12-4
         * Description  ：Input command and get the returned information,
         *                divide strings and filter out needed information
         * Parameter    ：none
         * Return       ：MemoryUsage m_memory_usage
         */
        MemoryUsage* select_memusage();
        /*
         * Function name：show_memusage
         * Author       ：marcie
         * Date         ：2018-12-4
         * Description  ：output memory usage infomation by protobuf message
         * Parameter    ：none
         * Output       :memory usage information
         * Return       ：none
         */
        void show_memusage();

        /* cpu function */
        typedef struct CPUTime
        {
            char cpu_name[20];
            unsigned int user_time;
            unsigned int nice_time;
            unsigned int system_time;
            unsigned int idle_time;
        } CpuOccupy ;
        void get_cpu_used_info (CpuOccupy *o) ;
        CPUUsage* cal_cpu_usage (CpuOccupy *first_info, CpuOccupy *second_info) ;

        /*netspeed struct*/
        typedef struct NetMessage
        {
            char netcard_name[20];
            long int save_rate;
        }NetMessage;

        long int get_net_used_info(NetMessage *net);
        NetUsage *cal_net_usage(NetMessage *first_time, NetMessage *last_time);


        RuntimeResourceUsage();

        virtual ~RuntimeResourceUsage();

    };
}



#endif //CHAMELEON_RUNTIME_RESOURCES_COLLECTOR_HPP
