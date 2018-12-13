/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguo 2573777501@qq.com
 * Date       ：18-12-10
 * Description：master flags
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
    class MasterFlagsBase:public virtual FlagsBase{
    public:
        string  master_port;

        MasterFlagsBase() {
            /*master run port*/
            add(&MasterFlagsBase::master_port,
                "port",
                "master port");
        }
    };
}