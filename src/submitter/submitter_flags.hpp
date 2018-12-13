/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguo 2573777501@qq.com
 * Date       ：18-12-10
 * Description：slave flags
 */
#include <iostream>

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

namespace chameleon {
    class SubmitterFlagsBase : public virtual FlagsBase {
    public:
        string spark_path;
        string master_ip_and_port;
        int submitter_run_port;

        SubmitterFlagsBase() {
            /*program run path*/
            add(&SubmitterFlagsBase::spark_path,
                "spath",
                "spark exists path \n",
                "example:/home/XXX/Downloads/spark-2.3.0-bin-hadoop2.7.tgz"
                 );

            /*program output path*/

            add(&SubmitterFlagsBase::master_ip_and_port,
                "masterinfo",
                "master ip and port \n"
                "example: ip:port"
                );

            add(&SubmitterFlagsBase::submitter_run_port,
                "port",
                "submitter run port",
                0);
        }
    };
}