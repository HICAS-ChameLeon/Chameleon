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

    add(&Flags::docker_socket, "docker_socket",
        "Resource used by the agent and the executor to provide CLI access\n"
        "to the Docker daemon. On Unix, this is typically a path to a\n"
        "socket, such as '/var/run/docker.sock'. On Windows this must be a\n"
        "named pipe, such as '//./pipe/docker_engine'. NOTE: This must be\n"
        "the path used by the Docker image used to run the agent.\n",
        "/var/run/docker.sock");

    add(&Flags::ht, "ht", "Heartbeat", 6);

    add(&Flags::master, "master", "The master ip and port info");

    add(&Flags::port, "port", "Port", 6061);

    add(&Flags::work_dir, "work_dir", "The path to store download file");
}