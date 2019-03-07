/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author      : Heldon 764165887@qq.com
 * Date       ：19-03-04
 * Description：slave flags
 */

#ifndef CHAMELEON_FLAGS_HPP
#define CHAMELEON_FLAGS_HPP

#include <string>

//stout denpendencies
#include <stout/option.hpp>
#include <stout/flags.hpp>

namespace chameleon{
    namespace slave{
        class Flags : virtual flags::FlagsBase{
        public:
            Flags();
            Option<std::string> docker_chameleon_image;
        };
    }
}



#endif //CHAMELEON_FLAGS_HPP
