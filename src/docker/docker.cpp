
#include <map>
#include <vector>

#include <stout/error.hpp>
#include <stout/foreach.hpp>
#include <stout/json.hpp>
#include <stout/lambda.hpp>
#include <stout/os.hpp>
#include <stout/path.hpp>
#include <stout/result.hpp>
#include <stout/strings.hpp>
#include <stout/stringify.hpp>

#include <stout/os/constants.hpp>
#include <stout/os/killtree.hpp>
#include <stout/os/read.hpp>
#include <stout/os/write.hpp>

#include <process/check.hpp>
#include <process/collect.hpp>
#include <process/io.hpp>

//#include "common/status_utils.hpp"

#include "docker.hpp"

#include "linux/cgroups.hpp"

using namespace mesos;

using namespace process;

using std::list;
using std::map;
using std::string;
using std::vector;

constexpr size_t DOCKER_PS_MAX_INSPECT_CALLS = 100;

template <typename T>
static Future<T> failure(
    const string& cmd,
    int status,
    const string& err)
{
  return Failure(
      "Failed to run '" + cmd + "': " + "Heldon wrong status" +
      "; stderr='" + err + "'");
}

static Future<Nothing> _checkError(const string& cmd, const Subprocess& s)
{
    Option<int> status = s.status().get();
    if (status.isNone()) {
        return Failure("No status found for '" + cmd + "'");
    }

    if (status.get() != 0) {
        // TODO(tnachen): Consider returning stdout as well.
        CHECK_SOME(s.err());
        return io::read(s.err().get())
                .then(lambda::bind(failure<Nothing>, cmd, status.get(), lambda::_1));
    }

    return Nothing();
}

static Future<Nothing> checkError(const string& cmd, const Subprocess& s)
{
    return s.status()
            .then(lambda::bind(_checkError, cmd, s));
}

Try<Owned<Docker>> Docker::create(
    const string& path,
    const string& socket,
    bool validate,
    const Option<JSON::Object>& config)
{
  Owned<Docker> docker(new Docker(path, socket, config));
  if (!validate) {
    return docker;
  }
  
  // Make sure that cgroups are mounted, and at least the 'cpu'
  // subsystem is attached.
  Result<string> hierarchy = cgroups::hierarchy("cpu");

  if (hierarchy.isNone()) {
    return Error("Failed to find a mounted cgroups hierarchy "
                 "for the 'cpu' subsystem; you probably need "
                 "to mount cgroups manually");
  }

  return docker;
}

void commandDiscarded(const Subprocess& s, const string& cmd)
{
  LOG(INFO) << "Heldon" << "'" << cmd << "' is being discarded";
  os::killtree(s.pid(), SIGKILL);
}

//docker run
Future<Option<int>> Docker::run(
        const Docker::RunOptions& options,
        const process::Subprocess::IO& _stdout,
        const process::Subprocess::IO& _stderr) const
{
  vector<string> argv;
  argv.push_back(path);
  argv.push_back("-H");
  argv.push_back(socket);
  argv.push_back("run");

  if (options.privileged) {
    argv.push_back("--privileged");
  }

  if (options.cpuShares.isSome()) {
    argv.push_back("--cpu-shares");
    argv.push_back(stringify(options.cpuShares.get()));
  }

  if (options.cpuQuota.isSome()) {
    argv.push_back("--cpu-quota");
    argv.push_back(stringify(options.cpuQuota.get()));
  }

  if (options.memory.isSome()) {
    argv.push_back("--memory");
    argv.push_back(stringify(options.memory->bytes()));
  }

  foreachpair(const string& key, const string& value, options.env) {
              argv.push_back("-e");
              argv.push_back(key + "=" + value);
            }

  foreach(const string& volume, options.volumes) {
    argv.push_back("-v");
    argv.push_back(volume);
  }

  if (options.volumeDriver.isSome()) {
    argv.push_back("--volume-driver=" + options.volumeDriver.get());
  }

  if (options.network.isSome()) {
    const string& network = options.network.get();
    argv.push_back("--net");
    argv.push_back(network);
  }

  if (options.hostname.isSome()) {
    argv.push_back("--hostname");
    argv.push_back(options.hostname.get());
  }

  foreach (const Docker::PortMapping& mapping, options.portMappings) {
    argv.push_back("-p");

    string portMapping = stringify(mapping.hostPort) + ":" +
                         stringify(mapping.containerPort);

    if (mapping.protocol.isSome()) {
      portMapping += "/" + strings::lower(mapping.protocol.get());
    }

    argv.push_back(portMapping);
  }

  foreach (const Device& device, options.devices) {
    if (!device.hostPath.absolute()) {
      return Failure("Device path '" + device.hostPath.string() + "'"
                                                                  " is not an absolute path");
    }

    string permissions;
    permissions += device.access.read ? "r" : "";
    permissions += device.access.write ? "w" : "";
    permissions += device.access.mknod ? "m" : "";

    // Docker doesn't handle this case (it fails by saying
    // that an absolute path is not being provided).
    if (permissions.empty()) {
      return Failure("At least one access required for --devices:"
                     " none specified for"
                     " '" + device.hostPath.string() + "'");
    }

    // Note that docker silently does not handle default devices
    // passed in with restricted permissions (e.g. /dev/null), so
    // we don't bother checking this case either.
    argv.push_back(
            "--device=" +
            device.hostPath.string() + ":" +
            device.containerPath.string() + ":" +
            permissions);
  }

  if (options.entrypoint.isSome()) {
    argv.push_back("--entrypoint");
    argv.push_back(options.entrypoint.get());
  }

  if (options.name.isSome()) {
    argv.push_back("--name");
    argv.push_back(options.name.get());
  }

  foreach (const string& option, options.additionalOptions) {
    argv.push_back(option);
  }

  argv.push_back(options.image);

  foreach(const string& argument, options.arguments) {
    argv.push_back(argument);
  }

  string cmd = strings::join(" ", argv);

  LOG(INFO) << "Running " << cmd;

  Try<Subprocess> s = subprocess(
          path,
          argv,
          Subprocess::PATH(os::DEV_NULL),
          _stdout,
          _stderr,
          nullptr);

  if (s.isError()) {
    return Failure("Failed to create subprocess '" + path + "': " + s.error());
  }
  s->status().onDiscard(lambda::bind(&commandDiscarded, s.get(), cmd));

  return s->status();
}

//docker inspect
Future<Docker::Container> Docker::inspect(
        const string& containerName,
        const Option<Duration>& retryInterval) const
{
    Owned<Promise<Docker::Container>> promise(new Promise<Docker::Container>());

    const string cmd =  path + " -H " + socket + " inspect " + containerName;
    _inspect(cmd, promise, retryInterval);

    return promise->future();
}
void Docker::_inspect(
        const string& cmd,
        const Owned<Promise<Docker::Container>>& promise,
        const Option<Duration>& retryInterval)
{
    if (promise->future().hasDiscard()) {
        promise->discard();
        return;
    }

    VLOG(1) << "Running " << cmd;

    Try<Subprocess> s = subprocess(
            cmd,
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PIPE(),
            Subprocess::PIPE());

    if (s.isError()) {
        promise->fail("Failed to create subprocess '" + cmd + "': " + s.error());
        return;
    }

    // Start reading from stdout so writing to the pipe won't block
    // to handle cases where the output is larger than the pipe
    // capacity.
    const Future<string> output = io::read(s.get().out().get());

    s.get().status()
            .onAny([=]() { __inspect(cmd, promise, retryInterval, output, s.get()); });
}


void Docker::__inspect(
        const string& cmd,
        const Owned<Promise<Docker::Container>>& promise,
        const Option<Duration>& retryInterval,
        Future<string> output,
        const Subprocess& s)
{
    if (promise->future().hasDiscard()) {
        promise->discard();
        output.discard();
        return;
    }

    // Check the exit status of 'docker inspect'.
    CHECK_READY(s.status());

    Option<int> status = s.status().get();

    if (!status.isSome()) {
        promise->fail("No status found from '" + cmd + "'");
    } else if (status.get() != 0) {
        output.discard();

        if (retryInterval.isSome()) {
            VLOG(1) << "Retrying inspect with non-zero status code. cmd: '"
                    << cmd << "', interval: " << stringify(retryInterval.get());
            Clock::timer(retryInterval.get(),
                         [=]() { _inspect(cmd, promise, retryInterval); } );
            return;
        }

        CHECK_SOME(s.err());
        io::read(s.err().get())
                .then(lambda::bind(
                        failure<Nothing>,
                        cmd,
                        status.get(),
                        lambda::_1))
                .onAny([=](const Future<Nothing>& future) {
                    CHECK_FAILED(future);
                    promise->fail(future.failure());
                });
        return;
    }

    // Read to EOF.
    CHECK_SOME(s.out());
    output
            .onAny([=](const Future<string>& output) {
                ___inspect(cmd, promise, retryInterval, output);
            });
}


void Docker::___inspect(
        const string& cmd,
        const Owned<Promise<Docker::Container>>& promise,
        const Option<Duration>& retryInterval,
        const Future<string>& output)
{
    if (promise->future().hasDiscard()) {
        promise->discard();
        return;
    }

    if (!output.isReady()) {
        promise->fail(output.isFailed() ? output.failure() : "future discarded");
        return;
    }

    Try<Docker::Container> container = Docker::Container::create(
            output.get());

    if (container.isError()) {
        promise->fail("Unable to create container: " + container.error());
        return;
    }

    if (retryInterval.isSome() && !container.get().started) {
        VLOG(1) << "Retrying inspect since container not yet started. cmd: '"
                << cmd << "', interval: " << stringify(retryInterval.get());
        Clock::timer(retryInterval.get(),
                     [=]() { _inspect(cmd, promise, retryInterval); } );
        return;
    }

    promise->set(container.get());
}

//docker stop
Future<Nothing> Docker::stop(
        const string& containerName,
        const Duration& timeout,
        bool remove) const
{
    int timeoutSecs = (int) timeout.secs();
    if (timeoutSecs < 0) {
        return Failure("A negative timeout cannot be applied to docker stop: " +
                       stringify(timeoutSecs));
    }

    string cmd = path + " -H " + socket + " stop -t " + stringify(timeoutSecs) +
                 " " + containerName;

    VLOG(1) << "Running " << cmd;

    Try<Subprocess> s = subprocess(
            cmd,
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PIPE());

    if (s.isError()) {
        return Failure("Failed to create subprocess '" + cmd + "': " + s.error());
    }

    return s.get().status()
            .then(lambda::bind(
                    &Docker::_stop,
                    *this,
                    containerName,
                    cmd,
                    s.get(),
                    remove));
}

Future<Nothing> Docker::_stop(
        const Docker& docker,
        const string& containerName,
        const string& cmd,
        const Subprocess& s,
        bool remove)
{
    Option<int> status = s.status().get();

    if (remove) {
        bool force = !status.isSome() || status.get() != 0;
        return docker.rm(containerName, force)
                .repair([=](const Future<Nothing>& future) {
                    LOG(ERROR) << "Unable to remove Docker container '"
                               << containerName + "': " << future.failure();
                    return Nothing();
                });
    }

    return checkError(cmd, s);
}

//docker rm (-f)
Future<Nothing> Docker::rm(
        const string& containerName,
        bool force) const
{
    // The `-v` flag removes Docker volumes that may be present.
    const string cmd =
            path + " -H " + socket +
            (force ? " rm -f -v " : " rm -v ") + containerName;

    VLOG(1) << "Running " << cmd;

    Try<Subprocess> s = subprocess(
            cmd,
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PIPE());

    if (s.isError()) {
        return Failure("Failed to create subprocess '" + cmd + "': " + s.error());
    }

    return checkError(cmd, s.get());
}

//docker ps -a
Future<list<Docker::Container>> Docker::ps(
        bool all,
        const Option<string>& prefix) const
{
    string cmd = path + " -H " + socket + (all ? " ps -a" : " ps");

    VLOG(1) << "Running " << cmd;

    Try<Subprocess> s = subprocess(
            cmd,
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PIPE(),
            Subprocess::PIPE());

    if (s.isError()) {
        return Failure("Failed to create subprocess '" + cmd + "': " + s.error());
    }

    // Start reading from stdout so writing to the pipe won't block
    // to handle cases where the output is larger than the pipe
    // capacity.
    const Future<string>& output = io::read(s.get().out().get());

    return s.get().status()
            .then(lambda::bind(&Docker::_ps, *this, cmd, s.get(), prefix, output));
}

Future<list<Docker::Container>> Docker::_ps(
        const Docker& docker,
        const string& cmd,
        const Subprocess& s,
        const Option<string>& prefix,
        Future<string> output)
{
    Option<int> status = s.status().get();

    if (!status.isSome()) {
        output.discard();
        return Failure("No status found from '" + cmd + "'");
    } else if (status.get() != 0) {
        output.discard();
        CHECK_SOME(s.err());
        return io::read(s.err().get())
                .then(lambda::bind(
                        failure<list<Docker::Container>>,
                        cmd,
                        status.get(),
                        lambda::_1));
    }

    // Read to EOF.
    return output.then(lambda::bind(&Docker::__ps, docker, prefix, lambda::_1));
}

Future<list<Docker::Container>> Docker::__ps(
        const Docker& docker,
        const Option<string>& prefix,
        const string& output)
{
    Owned<vector<string>> lines(new vector<string>());
    *lines = strings::tokenize(output, "\n");

    // Skip the header.
    CHECK(!lines->empty());
    lines->erase(lines->begin());

    Owned<list<Docker::Container>> containers(new list<Docker::Container>());

    Owned<Promise<list<Docker::Container>>> promise(
            new Promise<list<Docker::Container>>());

    // Limit number of parallel calls to docker inspect at once to prevent
    // reaching system's open file descriptor limit.
    inspectBatches(containers, lines, promise, docker, prefix);

    return promise->future();
}

void Docker::inspectBatches(
        Owned<list<Docker::Container>> containers,
        Owned<vector<string>> lines,
        Owned<Promise<list<Docker::Container>>> promise,
        const Docker& docker,
        const Option<string>& prefix)
{
    list<Future<Docker::Container>> batch =
            createInspectBatch(lines, docker, prefix);

    collect(batch).onAny([=](const Future<list<Docker::Container>>& c) {
        if (c.isReady()) {
            foreach (const Docker::Container& container, c.get()) {
                containers->push_back(container);
            }
            if (lines->empty()) {
                promise->set(*containers);
            }
            else {
                inspectBatches(containers, lines, promise, docker, prefix);
            }
        } else {
            if (c.isFailed()) {
                promise->fail("Docker ps batch failed " + c.failure());
            }
            else {
                promise->fail("Docker ps batch discarded");
            }
        }
    });
}

list<Future<Docker::Container>> Docker::createInspectBatch(
        Owned<vector<string>> lines,
        const Docker& docker,
        const Option<string>& prefix)
{
    list<Future<Docker::Container>> batch;

    while (!lines->empty() && batch.size() < DOCKER_PS_MAX_INSPECT_CALLS) {
        string line = lines->back();
        lines->pop_back();

        // Inspect the containers that we are interested in depending on
        // whether or not a 'prefix' was specified.
        vector<string> columns = strings::split(strings::trim(line), " ");

        // We expect the name column to be the last column from ps.
        string name = columns[columns.size() - 1];
        if (prefix.isNone() || strings::startsWith(name, prefix.get())) {
            batch.push_back(docker.inspect(name));
        }
    }

    return batch;
}

//docker pull
Future<Docker::Image> Docker::pull(
        const string& directory,
        const string& image,
        bool force) const
{
    vector<string> argv;

    string dockerImage = image;

    // Check if the specified image has a tag. Also split on "/" in case
    // the user specified a registry server (ie: localhost:5000/image)
    // to get the actual image name. If no tag was given we add a
    // 'latest' tag to avoid pulling down the repository.

    vector<string> parts = strings::split(image, "/");

    if (!strings::contains(parts.back(), ":")) {
        dockerImage += ":latest";
    }

    if (force) {
        // Skip inspect and docker pull the image.
        return Docker::__pull(*this, directory, image, path, socket, config);
    }

    argv.push_back(path);
    argv.push_back("-H");
    argv.push_back(socket);
    argv.push_back("inspect");
    argv.push_back(dockerImage);

    string cmd = strings::join(" ", argv);

    VLOG(1) << "Running " << cmd;

    Try<Subprocess> s = subprocess(
            path,
            argv,
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PIPE(),
            Subprocess::PIPE(),
            nullptr);

    if (s.isError()) {
        return Failure("Failed to create subprocess '" + cmd + "': " + s.error());
    }

    // Start reading from stdout so writing to the pipe won't block
    // to handle cases where the output is larger than the pipe
    // capacity.
    const Future<string> output = io::read(s.get().out().get());

    // We assume docker inspect to exit quickly and do not need to be
    // discarded.
    return s.get().status()
            .then(lambda::bind(
                    &Docker::_pull,
                    *this,
                    s.get(),
                    directory,
                    dockerImage,
                    path,
                    socket,
                    config,
                    output));
}

Future<Docker::Image> Docker::_pull(
        const Docker& docker,
        const Subprocess& s,
        const string& directory,
        const string& image,
        const string& path,
        const string& socket,
        const Option<JSON::Object>& config,
        Future<string> output)
{
    Option<int> status = s.status().get();
    if (status.isSome() && status.get() == 0) {
        return output
                .then(lambda::bind(&Docker::____pull, lambda::_1));
    }

    output.discard();

    return Docker::__pull(docker, directory, image, path, socket, config);
}

Future<Docker::Image> Docker::__pull(
        const Docker& docker,
        const string& directory,
        const string& image,
        const string& path,
        const string& socket,
        const Option<JSON::Object>& config)
{
    vector<string> argv;
    argv.push_back(path);
    argv.push_back("-H");
    argv.push_back(socket);
    argv.push_back("pull");
    argv.push_back(image);

    string cmd = strings::join(" ", argv);

    VLOG(1) << "Running " << cmd;

    // Set the HOME path where docker config file locates.
    Option<string> home;
    if (config.isSome()) {
        Try<string> _home = os::mkdtemp();

        if (_home.isError()) {
            return Failure("Failed to create temporary directory for docker config"
                           "file: " + _home.error());
        }

        home = _home.get();

        Result<JSON::Object> auths = config->find<JSON::Object>("auths");
        if (auths.isError()) {
            return Failure("Failed to find 'auths' in docker config file: " +
                           auths.error());
        }

        const string path = auths.isSome()
                            ? path::join(home.get(), ".docker")
                            : home.get();

        Try<Nothing> mkdir = os::mkdir(path);
        if (mkdir.isError()) {
            return Failure("Failed to create path '" + path + "': " + mkdir.error());
        }

        const string file = path::join(path, auths.isSome()
                                             ? "config.json"
                                             : ".dockercfg");

        Try<Nothing> write = os::write(file, stringify(config.get()));
        if (write.isError()) {
            return Failure("Failed to write docker config file to '" +
                           file + "': " + write.error());
        }
    }

    // Currently the Docker CLI picks up .docker/config.json (old
    // .dockercfg by looking for the config file in the $HOME
    // directory. The docker config file can either be specified by
    // the agent flag '--docker_config', or by one of the URIs
    // provided which is a docker config file we want docker to be
    // able to pick it up from the sandbox directory where we store
    // all the URI downloads.
    // TODO(gilbert): Deprecate the fetching docker config file
    // specified as URI method on 0.30.0 release.
    map<string, string> environment = os::environment();
    environment["HOME"] = directory;

    bool configExisted =
            os::exists(path::join(directory, ".docker", "config.json")) ||
            os::exists(path::join(directory, ".dockercfg"));

    // We always set the sandbox as the 'HOME' directory, unless
    // there is no docker config file downloaded in the sandbox
    // and another docker config file is specified using the
    // '--docker_config' agent flag.
    if (!configExisted && home.isSome()) {
        environment["HOME"] = home.get();
    }

    Try<Subprocess> s_ = subprocess(
            path,
            argv,
            Subprocess::PATH(os::DEV_NULL),
            Subprocess::PIPE(),
            Subprocess::PIPE(),
            nullptr,
            environment);

    if (s_.isError()) {
        return Failure("Failed to execute '" + cmd + "': " + s_.error());
    }

    // Docker pull can run for a long time due to large images, so
    // we allow the future to be discarded and it will kill the pull
    // process.
    return s_.get().status()
            .then(lambda::bind(
                    &Docker::___pull,
                    docker,
                    s_.get(),
                    cmd,
                    directory,
                    image))
            .onDiscard(lambda::bind(&commandDiscarded, s_.get(), cmd))
            .onAny([home]() {
                if (home.isSome()) {
                    Try<Nothing> rmdir = os::rmdir(home.get());

                    if (rmdir.isError()) {
                        LOG(WARNING) << "Failed to remove docker config file temporary"
                                     << "'HOME' directory '" << home.get() << "': "
                                     << rmdir.error();
                    }
                }
            });
}

Future<Docker::Image> Docker::___pull(
        const Docker& docker,
        const Subprocess& s,
        const string& cmd,
        const string& directory,
        const string& image)
{
    Option<int> status = s.status().get();

    if (!status.isSome()) {
        return Failure("No status found from '" + cmd + "'");
    } else if (status.get() != 0) {
        return io::read(s.err().get())
                .then(lambda::bind(&failure<Image>, cmd, status.get(), lambda::_1));
    }

    // We re-invoke Docker::pull in order to now do an 'inspect' since
    // the image should be present (see Docker::pull).
    // TODO(benh): Factor out inspect code from Docker::pull to be
    // reused rather than this (potentially infinite) recursive call.
    return docker.pull(directory, image);
}

Future<Docker::Image> Docker::____pull(
        const string& output)
{
    Try<JSON::Array> parse = JSON::parse<JSON::Array>(output);

    if (parse.isError()) {
        return Failure("Failed to parse JSON: " + parse.error());
    }

    JSON::Array array = parse.get();

    // Only return if only one image identified with name.
    if (array.values.size() == 1) {
        CHECK(array.values.front().is<JSON::Object>());

        Try<Docker::Image> image =
                Docker::Image::create(array.values.front().as<JSON::Object>());

        if (image.isError()) {
            return Failure("Unable to create image: " + image.error());
        }

        return image.get();
    }

    // TODO(tnachen): Handle the case where the short image ID was
    // not sufficiently unique and 'array.values.size() > 1'.

    return Failure("Failed to find image");
}
