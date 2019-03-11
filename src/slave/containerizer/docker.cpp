/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author      : Heldon 764165887@qq.com
 * Date       ：19-03-01
 * Description：containerizer(docker) codes
 */

//C++11 dependencies
#include <stdlib.h>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

//stout denpendencies
#include <stout/os.hpp>
#include <stout/duration.hpp>

//libprocess dependencies
#include <process/dispatch.hpp>
#include <process/defer.hpp>

//chameleon dependencies
#include "docker.hpp"
//#include "slave_flags.hpp"
#include "docker/docker.hpp"
#include "docker/docker.cpp"
#include "common/status_utils.hpp"

using std::map;
using std::string;
using namespace process;

namespace chameleon{
namespace slave{
    //destructor function
    DockerContainerizer::~DockerContainerizer() {
        terminate(m_process.get());
        process::wait(m_process.get());
    }

    Try<DockerContainerizer*> DockerContainerizer::create() {
        Try<Owned<Docker>> create = Docker::create(
                "docker",
                "/var/run/docker.sock",
                false);

        if (create.isError()) {
            return Error("Failed to create docker: " + create.error());
        }

        Shared<Docker> docker = create->share();

        return new chameleon::slave::DockerContainerizer(docker);
    }

    DockerContainerizer::DockerContainerizer(Shared<Docker> docker)
            : m_process(new chameleon::slave::DockerContainerizerProcess(docker)) {
        spawn(m_process.get());
    }

    /**
    * Function name  : pull
    * Author         : Heldon
    * Date           : 2019-03-05
    * Description    : pull the image
    * Return         : Future<Nothing>
    */
    Future<Nothing> DockerContainerizerProcess::pull(const mesos::ContainerID& container_id){
        if(!m_containers.contains(container_id)){
            return Failure("Container is already destroyed");
        }

        Container* container = m_containers.at(container_id);
        string image = container->image();

        Future<Docker::Image> future = m_docker->pull(
                container->m_directory,
                image,
                false);  //not force pull

        m_containers.at(container_id)->m_pull = future;

        return future.then(defer(self(),[=](){
            LOG(INFO)<<"Docker pull "<< image << " completed";
            return Nothing();
        }));
    }

    /**
    * Function name  : create
    * Author         : Heldon
    * Date           : 2019-03-04
    * Description    : create a container
    * Return         : Try<DockerContainerizerProcess::Container*>
    */
    Try<DockerContainerizerProcess::Container*> DockerContainerizerProcess::Container::create(
            const mesos::ContainerID& container_id,
            const Option<mesos::TaskInfo>& taskInfo,
            const mesos::ExecutorInfo& executorInfo,
            const string& directory,
            const Option<string>& user,
            const mesos::SlaveID& slaveId,
            const map<string, string>& environment){

        Try<Nothing> touch = os::touch(path::join(directory, "stdout"));

        if (touch.isError()) {
            return Error("Failed to touch 'stdout': " + touch.error());
        }

        touch = os::touch(path::join(directory, "stderr"));

        string containerWorkdir = directory;

        if (touch.isError()) {
            return Error("Failed to touch 'stderr': " + touch.error());
        }

        Option<mesos::ContainerInfo> containerInfo = None();
        Option<mesos::CommandInfo> commandInfo = None();

        return new Container(
                container_id,
                taskInfo,
                executorInfo,
                containerWorkdir,
                user,
                slaveId,
                commandInfo,
                containerInfo,
                environment);
    }

    /**
    * Function name  : launch
    * Author         : Heldon
    * Date           : 2019-03-01
    * Description    : launch the container(docker)
    * Return         : Future<bool>
    */
    Future<bool> DockerContainerizer::launch(
            const mesos::ContainerID& container_id,
            const Option<mesos::TaskInfo>& taskInfo,
            const mesos::ExecutorInfo& executorInfo,
            const std::string directory,
            const Option<std::string>& user,
            const mesos::SlaveID& slaveId,
            const std::map<std::string, std::string>& environment){
        LOG(INFO)<<"Heldon Enter function launch";
        return dispatch(
                m_process.get(),
                &DockerContainerizerProcess::launch,
                container_id,
                taskInfo,
                executorInfo,
                directory,
                user,
                slaveId,
                environment);
    }

    /**
    * Function name  : launch
    * Author         : Heldon
    * Date           : 2019-03-01
    * Description    : the process of launch the container(docker)
    * Return         : Future<bool>
    */
    Future<bool> DockerContainerizerProcess::launch(
            const mesos::ContainerID& container_id,
            const Option<mesos::TaskInfo>& taskInfo,
            const mesos::ExecutorInfo& executorInfo,
            const string& directory,
            const Option<string>& user,
            const mesos::SlaveID& slaveId,
            const map<string, string>& environment){

        Option<mesos::ContainerInfo> containerInfo;

        //assign the containerInfo
        if(taskInfo.isSome() && taskInfo.get().has_container()){
            containerInfo = taskInfo.get().container();
        }
        else if(executorInfo.has_container()){
            containerInfo = executorInfo.container();
        }

        if(containerInfo.isNone()){
            LOG(INFO)<<"No containerInfo found";
            return false;
        }

        if(containerInfo.get().type() != mesos::ContainerInfo::DOCKER){
            LOG(INFO)<<"container type is not DOCKER";
            return false;
        }

        Try<Container*> container = Container::create(
                container_id,
                taskInfo,
                executorInfo,
                directory,
                user,
                slaveId,
                environment);

        if (container.isError()) {
            return Failure("Failed to create container: " + container.error());
        }

        m_containers[container_id] = container.get();

        if (taskInfo.isSome()) {
            LOG(INFO) << "Starting container '" << container_id.value()
                      << "' for task '" << taskInfo.get().task_id().value()
                      << "' (and executor '" << executorInfo.executor_id().value()
                      << "') of framework " << executorInfo.framework_id().value();
        } else {
            LOG(INFO) << "Starting container '" << container_id.value()
                      << "' for executor '" << executorInfo.executor_id().value()
                      << "' and framework " << executorInfo.framework_id().value();
        }

        Future<Nothing> f = Nothing();

        return f.then(defer(
                self(),
                &Self::_launch,
                container_id,
                taskInfo,
                executorInfo,
                directory,
                slaveId));
    }

    /**
    * Function name  : _launch
    * Author         : Heldon
    * Date           : 2019-03-05
    * Description    : the process of launch the container(docker)
    * Return         : Future<bool>
    */
    Future<bool> DockerContainerizerProcess::_launch(
            const mesos::ContainerID& container_id,
            const Option<mesos::TaskInfo>& taskInfo,
            const mesos::ExecutorInfo& executorInfo,
            const string& directory,
            const mesos::SlaveID& slaveId
            ){
        Container* container = m_containers.at(container_id);
        string container_name = container->name();

        return container->m_launch = pull(container_id)
                .then(defer(self(), [=](){
            return launchExecutorContainer(container_id, directory);
        }))
        .then(defer(self(), [=](const Docker::Container& docker_container){
            return Future<Docker::Container>(docker_container);
        }))
        .then(defer(self(), [=](){
            return true;
        }));
    }

    /**
    * Function name  : launchExecutorContainer
    * Author         : Heldon
    * Date           : 2019-03-05
    * Description    : the process of launch the container(docker)
    * Return         : Future<Docker::Container>
    */
    Future<Docker::Container> DockerContainerizerProcess::launchExecutorContainer(
            const mesos::ContainerID& container_id,
            const string& container_name){
        if(!m_containers.contains(container_id)){
            return Failure("Container is already destroyed");
        }

        Container* container = m_containers.at(container_id);

        Try<Docker::RunOptions> runOptions = Docker::RunOptions::create(
                container->m_container,
                container->m_command,
                container_name,
                container->m_directory,
                "/mnt/chame/sandbox",
                container->m_resources,
                false,
                container->m_environment,
                None());

        if (runOptions.isError()) {
            return Failure(runOptions.error());
        }

        //Start the executor in a Docker container
        Future<Option<int>> run = m_docker->run(runOptions.get());

        auto promise = std::make_shared<Promise<Docker::Container>>();

        Future<Docker::Container> inspect = m_docker->inspect(container_name, Seconds(1));

        inspect.onAny([=](Future<Docker::Container> container) {
            promise->associate(container);
        });

        run.onAny([=]() mutable{
            if(!run.isReady()){
                inspect.discard();
                promise->fail(run.isFailed() ? run.failure() : "discarded");
            }
            else if(run->isNone()){
                inspect.discard();
                promise->fail("Failed to obtain exit status of container");
            }
            else{
                if(WIFEXITED(run->get()) && WEXITSTATUS(run->get()) == 0){
                    inspect.discard();
                    promise->fail("Container" + WSTRINGIFY(run->get()));
                }
            }
        });
        return promise->future();
    }
};
}