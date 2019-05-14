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

#include <docker.hpp>
#include <resources.hpp>
#include <cgroups.hpp>
#include <cgroups.cpp>



using namespace process;

using std::list;
using std::string;
using std::vector;
using std::cout;
using std::endl;

static const string NAME_PREFIX="mesos-docker";

int main(){

    const string containerName = NAME_PREFIX + "-test";
    //limit the resource
    mesos::Resources resources = mesos::Resources::parse("cpus:1;mem:512").get();

    Owned<Docker> docker = Docker::create("docker","/var/run/docker.sock",false).get();
    std::cout << docker.get()->getPath() << std::endl;

    Future<list<Docker::Container>> containers = docker->ps(true, containerName); // ps -a
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
    cout << "path : " << path << endl;
    Try<string> mkdtemp = os::mkdtemp(path);
    Try<string> directory = mkdtemp;

    mesos::ContainerInfo containerInfo;
    containerInfo.set_type(mesos::ContainerInfo::DOCKER);

    mesos::ContainerInfo::DockerInfo dockerInfo;
    dockerInfo.set_image("hello-world");
    containerInfo.mutable_docker()->CopyFrom(dockerInfo);

    mesos::CommandInfo commandInfo;
    //commandInfo.set_value("sleep 120");

    //hello-world image doesn't need shell, so we set it to be false
    commandInfo.set_shell(false);

    Try<Docker::RunOptions> runOptions = Docker::RunOptions::create(
            containerInfo,
            commandInfo,
            containerName,
            directory.get(),
            "/mnt/chameleon/sandbox",
            resources);

    //add additional options
    //runOptions.get().additionalOptions.push_back("-i");
    //runOptions.get().additionalOptions.push_back("-t");

    // Start the container.
    Future<Option<int>> status = docker->run(runOptions.get());
    status.await();

    Future<Docker::Container> inspect = docker->inspect(containerName, Seconds(1));

    inspect.await();

    //we should be able to see the container now
    containers = docker->ps();  //ps
    containers.await();
    foreach (const Docker::Container& container, containers.get()) {
        std::cout<< "Heldon containers name : " << container.name <<std::endl;
    }

    // Stop the container.
    Future<Nothing> stop = docker->stop(containerName);
    stop.await();

    //the container should not appear in the result of ps()
    //but it should appear in the result of ps(true)
    containers = docker->ps();
    containers.await();
    cout << "it should be 0 : " << containers.get().size() << endl;

    containers = docker->ps(true, containerName);
    containers.await();
    cout << "int should have value : " << containers.get().size() << endl;
    foreach (const Docker::Container& container, containers.get()) {
        std::cout<< "Heldon containers name (by ps -a): " << container.name <<std::endl;
    }

    //check container's info, both id and name should remain the same since we haven't removed it
    //but pid should be none since it's not running
    inspect = docker->inspect(containerName);
    inspect.await();
    cout << "id : " << inspect->id << "name : " << inspect->name <<endl;
    cout << "pid : " << inspect->pid.isSome() << endl;
    cout << "inspecet output" << inspect->output << endl;
//    //remove the container
//    Future<Nothing> rm = docker->rm("mesos-test");
//    rm.await();
//
//    //should not be able to inspect the container.
//    inspect = docker->inspect("mesos-test");
//    if(inspect.isFailed()){
//        cout << "inspect is failed" << endl;
//    }
//
//    // we should not able to see the container by ps(true)
//    containers = docker->ps(true, "mesos-test");
//    cout << "container's size should be 0 : " << containers.get().size() << endl;



}

