/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguow 2573777501@qq.com
 * Date       ：19-3-16
 * Description：Three scheduling algorithms are provided
 *              (1) Based on the CPU usage
 *              (2) Based on the MEMORY remaining size
 *              (3) Based on the Mixed judgment of multiple resources
 */
#ifndef CHAMELEON_SCHEDULER_HPP
#define CHAMELEON_SCHEDULER_HPP

// C++ 11 dependencies
#include <vector>
#include <set>
#include<iterator>
#include <sstream>
#include <unordered_map>

// google
#include <glog/logging.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/gtest.hpp>
#include <stout/json.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>
#include <stout/os.hpp>
#include <stout/uuid.hpp>


// libprocess dependencies
#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/delay.hpp>
#include <process/subprocess.hpp>

// protobuf
#include <super_master_related.pb.h>
#include <hardware_resource.pb.h>
#include <runtime_resource.pb.h>
#include <messages.pb.h>

// chameleon headers
#include <configuration_glog.hpp>
#include "../master.hpp"


using std::string;
using std::set;
using std::vector;
using std::unordered_map;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;
using process::Subprocess;
using process::subprocess;
using process::http::Request;
using process::http::OK;
using process::http::Response;

using namespace process::http;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;

namespace chameleon {
    class Scheduler {
    public:
        explicit Scheduler() {}

        virtual void initialize();

        virtual ~Scheduler();

        virtual Try<string> scheduler(unordered_map<string, HardwareResourcesMessage> hardware_resource,
                unordered_map<string, RuntimeResourcesMessage> runtime_resource) = 0;

    };

    class CpuScheduler : public Scheduler {
    public:
        Try<string> scheduler(unordered_map<string, RuntimeResourcesMessage> runtime_resource) override;
    };

    class MemoryScheduler : public Scheduler {
    public:
        Try<string> scheduler(unordered_map<string, RuntimeResourcesMessage> runtime_resource) override;
    };

    class MetricScheduler : public scheduler {
        Try<string> scheduler(unordered_map<string, HardwareResourcesMessage> hardware_resource,
                              unordered_map<string, RuntimeResourcesMessage> runtime_resource) override;
    };
}

#endif

