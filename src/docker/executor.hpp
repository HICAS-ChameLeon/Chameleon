
#ifndef __DOCKER_EXECUTOR_HPP__
#define __DOCKER_EXECUTOR_HPP__

#include <stdio.h>

#include <map>
#include <string>

#include <process/process.hpp>

#include <stout/option.hpp>

#include <flags.hpp>
#include <logging.hpp>

namespace chameleon {
namespace docker {

struct Flags : public virtual chameleon::logging::Flags
{
  Flags() {
    add(&Flags::container,
        "container",
        "The name of the docker container to run.");

    add(&Flags::docker,
        "docker",
        "The path to the docker executable.");

    add(&Flags::docker_socket,
        "docker_socket",
        "Resource used by the agent and the executor to provide CLI access\n"
        "to the Docker daemon. On Unix, this is typically a path to a\n"
        "socket, such as '/var/run/docker.sock'. On Windows this must be a\n"
        "named pipe, such as '//./pipe/docker_engine'.");

    add(&Flags::sandbox_directory,
        "sandbox_directory",
        "The path to the container sandbox holding stdout and stderr files\n"
        "into which docker container logs will be redirected.");

    add(&Flags::mapped_directory,
        "mapped_directory",
        "The sandbox directory path that is mapped in the docker container.");

    // TODO(alexr): Remove this after the deprecation cycle (started in 1.0).
    add(&Flags::stop_timeout,
        "stop_timeout",
        "The duration for docker to wait after stopping a running container\n"
        "before it kills that container. This flag is deprecated; use task's\n"
        "kill policy instead.");

    add(&Flags::launcher_dir,
        "launcher_dir",
        "Directory path of Mesos binaries. Mesos would find fetcher,\n"
        "containerizer and executor binary files under this directory.");

    add(&Flags::task_environment,
        "task_environment",
        "A JSON map of environment variables and values that should\n"
        "be passed into the task launched by this executor.");

    add(&Flags::cgroups_enable_cfs,
        "cgroups_enable_cfs",
        "Cgroups feature flag to enable hard limits on CPU resources\n"
        "via the CFS bandwidth limiting subfeature.\n",
        false);
  }

  Option<std::string> container;
  Option<std::string> docker;
  Option<std::string> docker_socket;
  Option<std::string> sandbox_directory;
  Option<std::string> mapped_directory;
  Option<std::string> launcher_dir;
  Option<std::string> task_environment;

  bool cgroups_enable_cfs;

  // TODO(alexr): Remove this after the deprecation cycle (started in 1.0).
  Option<Duration> stop_timeout;
};

} // namespace docker {
} // namespace chameleon {

#endif // __DOCKER_EXECUTOR_HPP__
