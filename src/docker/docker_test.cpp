//
// Created by heldon on 19-1-17.
//

#include <list>
#include <string>
#include <vector>

#include <process/future.hpp>
#include <process/gtest.hpp>
#include <process/owned.hpp>
#include <process/subprocess.hpp>

#include <stout/duration.hpp>
#include <stout/option.hpp>
#include <stout/gtest.hpp>

#include "docker/docker.hpp"
#include "docker/docker.cpp"
#include "mesos/resources.hpp"
#include "linux/cgroups.hpp"
#include "linux/cgroups.cpp"



using namespace process;

using std::list;
using std::string;
using std::vector;
using std::cout;
using std::endl;


int main(){
    //limit the resource
    //Resources resources = Resources::parse("cpus:1;mem:512").get();
    Owned<Docker> docker = Docker::create("docker","/var/run/docker.sock",false).get();
    //LOG(INFO)<< "Heldon docker path : "<< docker.get()->getPath();
    std::cout << docker.get()->getPath() << std::endl;
    Future<list<Docker::Container>> containers = docker->ps(true, "mesos-docker"); // ps -a
    containers.await();

    //此时并没有container
    if(containers.isReady()){
        cout<< "containers is ready" << endl;
    }
    else{
        cout<< "containers is not ready" << endl;
        cout<< containers.failure() << endl;
    }
    cout << containers.get().size() << endl;

    //add Container information
    const string& path = path::join(os::temp(), "XXXXXXXX");
    Try<string> mkdtemp = os::mkdtemp(path);
    Try<string> directory = mkdtemp;

    mesos::ContainerInfo containerInfo;
    containerInfo.set_type(mesos::ContainerInfo::DOCKER);

    mesos::ContainerInfo::DockerInfo dockerInfo;
    dockerInfo.set_image("alpine");
    containerInfo.mutable_docker()->CopyFrom(dockerInfo);

    mesos::CommandInfo commandInfo;
    commandInfo.set_value("sleep 120");

    Try<Docker::RunOptions> runOptions = Docker::RunOptions::create(
            containerInfo,
            commandInfo,
            "mesos-test",
            directory.get(),
            "/mnt/mesos/sandbox",
            None());

    // Start the container.
    Future<Option<int>> status = docker->run(runOptions.get());

    Future<Docker::Container> inspect =
            docker->inspect("mesos-test", Seconds(1));

    inspect.await();

    //we should be able to see the container now
    containers = docker->ps();  //ps
    containers.await();
    foreach (const Docker::Container& container, containers.get()) {
        std::cout<< "Heldon containers name : " << container.name <<std::endl;
    }

    // Stop the container.
    Future<Nothing> stop = docker->stop("mesos-test");
    stop.await();

    //the container should not appear in the result of ps()
    //but it should appear in the result of ps(true)
    containers = docker->ps();
    containers.await();
    cout << "it should be 0 : " << containers.get().size() << endl;

    containers = docker->ps(true, "mesos-test");
    containers.await();
    cout << "int should have value : " << containers.get().size() << endl;

    //check container's info, both id and name should remain the same since we haven't removed it
    //but pid should be none since it's not running
    inspect = docker->inspect("mesos-test");
    inspect.await();
    cout << "id : " << inspect->id << "name : " << inspect->name <<endl;

    //remove the container
}

