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

using namespace process;

using std::list;
using std::string;
using std::vector;

int main(){
    Try<Owned<Docker>> docker = Docker::create("docker","/var/run/docker.sock",false);
    
}

