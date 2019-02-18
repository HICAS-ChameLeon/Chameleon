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
    //Resources resources = Resources::parse("cpus:1;mem:512").get();
    Owned<Docker> docker = Docker::create("docker","/var/run/docker.sock",false).get();
    //LOG(INFO)<< "Heldon docker path : "<< docker.get()->getPath();
    std::cout << docker.get()->getPath() << std::endl;
    Future<list<Docker::Container>> containers = docker->ps(true, "mesos-docker"); // ps -a
    for(list<Docker::Container>::const_iterator i = containers.get().begin();i != containers.get().end(); ++i ){
        cout<< "Heldon containers id : "<< i->id << endl;
        cout<< "Heldon containers name : "<< i->name << endl;
    }
    //AWAIT_READY(containers);
    foreach (const Docker::Container& container, containers.get()) {
        cout<< "Heldon containers id : "<< container.id << endl;
        cout<< "Heldon containers name : "<< container.name << endl;
    }
}

