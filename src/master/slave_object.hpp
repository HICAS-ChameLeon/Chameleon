/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：20119-3-15
 * Description： slave_object
 */
#ifndef CHAMELEON_SLAVE_OBJECT_HPP
#define CHAMELEON_SLAVE_OBJECT_HPP

// C++ 11 dependencies
#include <iostream>
#include <vector>
#include <set>
#include <memory>
#include <unordered_map>

#include <boost/lexical_cast.hpp>

#include <glog/logging.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/gtest.hpp>
#include <stout/json.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>
#include <stout/hashmap.hpp>
#include <stout/uuid.hpp>
#include <stout/check.hpp>
#include <stout/boundedhashmap.hpp>
#include <stout/path.hpp>

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
#include <hardware_resource.pb.h>
#include <runtime_resource.pb.h>
#include <cluster_operation.pb.h>
#include <mesos.pb.h>
#include <scheduler.pb.h>
#include <messages.pb.h>
#include <super_master_related.pb.h>
#include <slave_related.pb.h>

// chameleon headers
#include <configuration_glog.hpp>
#include <chameleon_string.hpp>
#include <chameleon_os.hpp>
#include <chameleon_resources.hpp>

using std::string;
using std::set;
using std::vector;
using std::unordered_map;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::list;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;
//using namespace process::http;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;
using process::Subprocess;
namespace chameleon {

    class SlaveObject {
    public:
        explicit SlaveObject(const UPID& upid_,const HardwareResourcesMessage& hardware_resources_);

        ~ SlaveObject();
        const process::UPID m_upid;
        const string m_ip;
        const string m_hostname;
        const string m_uuid;
        const string m_upid_str;
        double m_available_cpus;
        double m_available_mem;
        uint32_t m_available_disk;
        HardwareResourcesMessage m_hardware_resources;

        // key: task_id  value: task_info
//        unordered_map<string , unique_ptr<mesos::TaskInfo> > m_running_tasks;
// key: framework_id  value: resources_of_framework
        unordered_map<string , ResourcesOfFramework> m_framework_resources;

        mesos::Offer* construct_a_offer(const string& offer_id, const mesos::FrameworkID &frameworkId);
        bool restore_resource_of_framework(const string& framework_id);

    private:
        SlaveObject(const SlaveObject&);
        SlaveObject& operator=(const SlaveObject&);
    };

}


#endif //CHAMELEON_SLAVE_OBJECT_HPP
