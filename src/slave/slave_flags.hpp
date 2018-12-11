/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguo 2573777501@qq.com
 * Date       ：18-12-10
 * Description：slave flags
 */

#include <stout/flags/flags.hpp>
#include <stout/os.hpp>
#include <stout/utils.hpp>

using flags::Flag;
using flags::FlagsBase;
using flags::Warnings;

using std::string;
using std::cout;
using std::endl;
using std::map;

using utils::arraySize;

namespace chameleon{
    class SlaveFlagsBase:public virtual FlagsBase{
    public:
        int slave_port;
        string master_ip;
        string master_hostname;

        SlaveFlagsBase() {
            /*slave run port*/
            add(&SlaveFlagsBase::slave_port,
                    "slave_port",
                    "set slave_port",
                    6061);

            /*master ip*/
            add(&SlaveFlagsBase::master_ip,
                    "master_ip",
                    "set master_ip",
                    6060);

            /*master hostname*/
            add(&SlaveFlagsBase::master_hostname,
                    "master_hostname",
                    "set master_hostname",
                    "lilelr");
        }
    };
}