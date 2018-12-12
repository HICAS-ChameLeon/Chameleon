/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：slave codes
 */
#ifndef CHAMELEON_PARTICIPANT_HPP
#define CHAMELEON_PARTICIPANT_HPP

// C++ 11 dependencies
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// stout dependencies
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>
#include <stout/path.hpp>
#include <stout/uuid.hpp>

#include <stout/os/getcwd.hpp>
#include <stout/os/write.hpp>

#include <stout/tests/utils.hpp>
#include <stout/gzip.hpp>


// libprocess dependencies
#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/delay.hpp>

// protobuf
#include <monitor_info.pb.h>
#include <job.pb.h>
#include <runtime_resource.pb.h>

// chameleon headers
#include <resource_collector.hpp>
#include <configuration_glog.hpp>
#include <runtime_resources_usage.hpp>

using std::string;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;

namespace chameleon {
    // forward declations
    class SlaveHeartbeater;

    constexpr Duration DEFAULT_HEARTBEAT_INTERVAL = Seconds(5);

    const string DEFAULT_MASTER = "master@172.20.110.228:6060";

    class Slave : public ProtobufProcess<Slave> {
    public:
        explicit Slave() : ProcessBase("slave"), m_interval(DEFAULT_HEARTBEAT_INTERVAL) {
            msp_resource_collector = make_shared<ResourceCollector>(ResourceCollector());
            msp_runtime_resource_usage = make_shared<RuntimeResourceUsage>(RuntimeResourceUsage());
            LOG(INFO) << "The heartbeat interval is " << DEFAULT_HEARTBEAT_INTERVAL;

//            msp_resource_collector = new ResourceCollector();
        }

        Slave(const Slave &slave) = default;

        virtual ~Slave() {
            LOG(INFO) << "~ Slave()";
        }

    protected:
        void finalize() override;

    public:
        virtual void initialize();

        void register_feedback(const string &hostname);

        void get_a_job(const UPID &master, const JobMessage &job_message);

        void send_heartbeat_to_master();

    private:
        shared_ptr<ResourceCollector> msp_resource_collector;
        shared_ptr<RuntimeResourceUsage> msp_runtime_resource_usage;
//        Option<process::Owned<SlaveHeartbeater>> heartbeater;
        shared_ptr<UPID> msp_masterUPID;
        const Duration m_interval;
        string m_uuid;
        void heartbeat();
    };


    class SlaveHeartbeater : public process::Process<SlaveHeartbeater> {

    public:

        SlaveHeartbeater(const Duration &interval)
                : process::ProcessBase(process::ID::generate("slaveheartbeater")),
                  m_interval(interval) {
        }

        virtual void initialize() {
            heartbeat();
//        install<Offer>(&Master::report_from_client, &Offer::key,&Offer::value);
        }

    private:

        void heartbeat() {
            DLOG(INFO) << "5 seconds";
            //  delays 5 seconds to invoke the function "heartbeat " of self.
            // it's cyclical because "heartbeat invoke heartbeat"
            process::delay(m_interval, self(), &Self::heartbeat);
        }

        const Duration m_interval;

    };
}


#endif //CHAMELEON_PARTICIPANT_HPP
