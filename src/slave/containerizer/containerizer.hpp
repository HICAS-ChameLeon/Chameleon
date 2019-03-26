/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author      : Heldon 764165887@qq.com
 * Date       ：19-03-01
 * Description：slave containerizer codes
 */

#ifndef CHAMELEON_CONTAINERIZER_HPP
#define CHAMELEON_CONTAINERIZER_HPP

// C++ 11 dependencies
#include <map>

// libprocess dependencies
#include <process/future.hpp>
#include <process/process.hpp>

// stout dependencies
#include <stout/option.hpp>

// protobuf
#include <mesos.pb.h>


namespace chameleon{

    class Containerizer {
    public:
        virtual ~Containerizer() {};

        virtual process::Future<bool> launch(
                const mesos::ContainerID& containerId,
                const Option<mesos::TaskInfo>& taskInfo,
                const mesos::ExecutorInfo& executorInfo,
                const std::string& directory,
                const mesos::SlaveID& slaveId,
                const std::map<std::string, std::string>& environment) = 0;
    };

}


#endif //CHAMELEON_CONTAINERIZER_HPP
