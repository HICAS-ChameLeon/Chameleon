/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author      : Heldon 764165887@qq.com
 * Date       ：19-03-01
 * Description：containerizer(docker) codes
 */

#ifndef CHAMELEON_DOCKER_HPP
#define CHAMELEON_DOCKER_HPP

//C++11 dependencies
#include <list>
#include <map>
#include <set>
#include <string>

//stout dependencies
#include <stout/flags.hpp>
#include <stout/os.hpp>

//google dependencies
#include <gflags/gflags.h>

//libprocess dependencies
#include <process/owned.hpp>
#include <process/shared.hpp>
#include <process/process.hpp>

//chameleon dependencies
#include <resources.hpp>
#include "slave_flags.hpp"
#include "docker/docker.hpp"
#include "docker/resources.hpp"

namespace chameleon{
namespace slave{
    //Foward declaration
    class DockerContainerizerProcess;

    class DockerContainerizer{
    public:

        static Try<DockerContainerizer*> create();

        DockerContainerizer(process::Shared<Docker> docker);

        virtual process::Future<bool> launch(
                const mesos::ContainerID& containerId,
                const Option<mesos::TaskInfo>& taskInfo,
                const mesos::ExecutorInfo& executorInfo,
                const std::string directory,
                const Option<std::string>& user,
                const mesos::SlaveID& slaveId,
                const std::map<std::string, std::string>& environment);

        virtual ~DockerContainerizer();

    private:
        process::Owned<DockerContainerizerProcess> m_process;
    };

    class DockerContainerizerProcess : public process::Process<DockerContainerizerProcess>{
    public:

        DockerContainerizerProcess(process::Shared<Docker> _docker) : m_docker(_docker){}

        //start launch containerizer
        virtual process::Future<bool> launch(
                const mesos::ContainerID& contaierId,
                const Option<mesos::TaskInfo>& taskInfo,
                const mesos::ExecutorInfo& executorInfo,
                const std::string& directory,
                const Option<std::string>& user,
                const mesos::SlaveID& slaveId,
                const std::map<std::string, std::string>& environment);

        // pull the image
        virtual process::Future<Nothing> pull(const mesos::ContainerID& containerId);

    private:

        process::Future<bool> _launch(
                const mesos::ContainerID& containerId,
                const Option<mesos::TaskInfo>& taskInfo,
                const mesos::ExecutorInfo& executorInfo,
                const std::string& directory,
                const mesos::SlaveID& slaveId);

        // Starts the executor in a Docker container.
        process::Future<Docker::Container> launchExecutorContainer(
                const mesos::ContainerID& containerId,
                const std::string& containerName);

        //const Flags m_flags;

        process::Shared<Docker> m_docker;

        struct Container{
            const mesos::ContainerID m_id;
            const Option<mesos::TaskInfo> m_task;
            const mesos::ExecutorInfo m_executor;
            mesos::ContainerInfo m_container;
            mesos::CommandInfo m_command;
            std::map<std::string, std::string> m_environment;
            Option<std::map<std::string, std::string>> m_taskEnvironment;

            // The sandbox directory for the container.
            std::string m_directory;

            const Option<std::string> m_user;
            mesos::SlaveID m_slaveId;
            //const Flags m_flags;
            mesos::Resources m_resources;
            process::Future<Docker::Image> m_pull;

            process::Future<bool> m_launch;

            static Try<Container*> create(
                    const mesos::ContainerID& id,
                    const Option<mesos::TaskInfo>& taskInfo,
                    const mesos::ExecutorInfo& executorInfo,
                    const std::string& directory,
                    const Option<std::string>& user,
                    const mesos::SlaveID& slaveId,
                    const std::map<std::string, std::string>& environment);

            static std::string name(const mesos::SlaveID& slaveId, const std::string& id) {
                return "chameleon-" + slaveId.value() + "." +
                       stringify(id);
            }

            Container(const mesos::ContainerID& id,
                      const Option<mesos::TaskInfo>& taskInfo,
                      const mesos::ExecutorInfo& executorInfo,
                      const std::string& directory,
                      const Option<std::string>& user,
                      const mesos::SlaveID& slaveId,
                      const Option<mesos::CommandInfo>& _command,
                      const Option<mesos::ContainerInfo>& _container,
                      const std::map<std::string, std::string>& _environment)
                    : m_id(id),
                      m_task(taskInfo),
                      m_executor(executorInfo),
                      m_environment(_environment),
                      m_directory(directory),
                      m_user(user),
                      m_slaveId(slaveId){

                m_resources = m_executor.resources();

                if (m_task.isSome()) {
                    CHECK(m_resources.contains(m_task.get().resources()));
                }

                if (_command.isSome()) {
                    m_command = _command.get();
                } else if (m_task.isSome()) {
                    m_command = m_task.get().command();
                } else {
                    m_command = m_executor.command();
                }

                if (_container.isSome()) {
                    m_container = _container.get();
                } else if (m_task.isSome()) {
                    m_container = m_task.get().container();
                } else {
                    m_container = m_executor.container();
                }
            }

            ~Container() {
                    os::rm(m_directory);
            }

            std::string name(){
                return name(m_slaveId, stringify(m_id));
            }

            std::string image() const{
                if(m_task.isSome()){
                    return m_task.get().container().docker().image();
                }
                return m_executor.container().docker().image();
            }
        };

        hashmap<mesos::ContainerID, Container*> m_containers;
    };
}

}


#endif //CHAMELEON_DOCKER_HPP
