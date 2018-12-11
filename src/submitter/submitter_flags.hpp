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
    class SubmitterFlagsBase:public virtual FlagsBase{
    public:
        string path;
        string p;

        SubmitterFlagsBase() {
            /*program run path*/
            add(&SubmitterFlagsBase::p,
                "program run path",
                "set run path",
                "~/spark/sbin/start-master.sh");

            /*program output path*/
            add(&SubmitterFlagsBase::path,
                    "program output path",
                    "set output path",
                    "~/output")
        }  76559876543fdrwq
    };
}