/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author      : Heldon 764165887@qq.com
 * Date       ：19-03-04
 * Description：slave flags
 */

//stout dependencies
#include <stout/flags.hpp>

#include "slave_flags.hpp"

chameleon::slave::Flags::Flags() {
    add(&Flags::docker_chameleon_image, "docker_chameleon_image", "The Docker image used to launch this Chameleon agent instance.");
}