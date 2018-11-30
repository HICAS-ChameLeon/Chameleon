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

// stout dependencies
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>

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

// chameleon headers
#include <resource_collector.hpp>

using std::cerr;
using std::cout;
using std::endl;
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
using namespace process::http;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;

namespace chameleon {
    // forward declations
    class SlaveHeartbeater;

    class Slave : public ProtobufProcess<Slave> {
    public:
        explicit Slave():ProcessBase("slave"){
            msp_resource_collector = make_shared<ResourceCollector>(ResourceCollector());
        }

        virtual ~Slave(){

        }

        virtual void initialize() {
            install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);
        }

        void register_feedback(const string& hostname){
            cout<<" receive register feedback from master"<< hostname<<endl;
        }

    private:
       shared_ptr<ResourceCollector> msp_resource_collector;
        Option<process::Owned<SlaveHeartbeater>> heartbeater;
    };

    constexpr Duration DEFAULT_HEARTBEAT_INTERVAL = Seconds(5);

    class SlaveHeartbeater : public process::Process<SlaveHeartbeater> {

    public:

        SlaveHeartbeater(const Duration& interval)
                : process::ProcessBase(process::ID::generate("myheartbeater")),
                  m_interval(interval) {
        }

        virtual void initialize() {
            heartbeat();
//        install<Offer>(&Master::report_from_client, &Offer::key,&Offer::value);
        }

    private:

        void heartbeat(){
            cout<<"5 seconds"<<endl;
            //  delays 5 seconds to invoke the function "heartbeat " of self.
            // it's cyclical because "heartbeat invoke heartbeat"
            process::delay(m_interval,self(),&Self::heartbeat);
        }

        const Duration m_interval;


    };
}




#endif //CHAMELEON_PARTICIPANT_HPP
