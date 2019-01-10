/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-12-06
 * Description：common configurations for different operation systems.
 */

#ifndef CHAMELEON_CHAMELEON_OS_HPP
#define CHAMELEON_CHAMELEON_OS_HPP
#include <string>
using std::string;
namespace chameleon {
    namespace setting{
        const string kx86Arch="x86_64";
        const string kArmArch="armv7l";
        const string MESOS_DIRECTORY="/home/heldon/chameleon/Chameleon/src/slave/mesos_executor/mesos-directory";
        const string MESOS_EXECUTOR_PATH="/home/heldon/chameleon/Chameleon/src/slave/mesos_executor/mesos-executor";
        const string FLAGS_LAUCHER_DIR="/home/heldon/chameleon/mesos/mesos-1.3.2/build/src";
    }
}



#endif //CHAMELEON_CHAMELEON_OS_HPP
