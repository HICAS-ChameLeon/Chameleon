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
    }

}



#endif //CHAMELEON_CHAMELEON_OS_HPP
