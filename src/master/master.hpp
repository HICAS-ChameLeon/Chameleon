/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */
#ifndef CHAMELEON_MASTER_HPP
#define CHAMELEON_MASTER_HPP
// C++ 11 dependencies
#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <memory>

#include <glog/logging.h>

// stout dependencies
#include <stout/gtest.hpp>
#include <stout/json.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>

// libprocess dependencies
#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>

// protobuf
#include <participant_info.pb.h>
#include <hardware_resource.pb.h>
#include <job.pb.h>
#include <runtime_resource.pb.h>

// chameleon headers
#include <configuration_glog.hpp>

using std::string;
using std::set;
using std::vector;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;
using namespace process::http;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;


namespace chameleon {

    class Master : public ProtobufProcess<Master> {

    public:
        UPID slave;

        explicit Master() : ProcessBase("master") {
            msp_spark_slave = make_shared<UPID>(UPID(test_slave_UPID));
            msp_spark_master = make_shared<UPID>(UPID(test_master_UPID));
        }

        virtual ~Master(){

        }

        virtual void initialize();


        void register_participant(const string& hostname);

        void update_hardware_resources(const UPID& from, const HardwareResourcesMessage& hardware_resources_message);

        void job_submited(const UPID& from, const JobMessage& job_message);

        void received_heartbeat(const UPID& slave, const RuntimeResourcesMessage& runtime_resouces_message);


    private:
        unordered_map<UPID,ParticipantInfo> m_participants;
        unordered_map<string,JSON::Object> m_hardware_resources;
        set<string> m_alive_slaves;
        unordered_map<string,JSON::Object> m_runtime_resources;
        unordered_map<string,RuntimeResourcesMessage> m_proto_runtime_resources;
//        unordered_map<string,HardwareResource> m_topology_resources;
        const string test_slave_UPID = "slave@172.20.110.79:6061";
        const string test_master_UPID = "slave@172.20.110.228:6061";
        shared_ptr<UPID> msp_spark_slave;
        shared_ptr<UPID> msp_spark_master;

        Try<string> find_min_cpu_and_memory_rates();
    };


}




#endif //CHAMELEON_MASTER_HPP
