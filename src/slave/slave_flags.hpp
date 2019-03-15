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

using std::string;

namespace chameleon{
    namespace slave{
        class Flags : public virtual flags::FlagsBase{
        public:
            Flags();
            Option<std::string> docker_chameleon_image;
            string docker_socket;
            int32_t port;
            uint32_t ht;
            string work_dir;
            Option<string> master;
        };
    }
}



#endif //CHAMELEON_FLAGS_HPP
