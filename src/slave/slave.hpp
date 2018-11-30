//
// Created by lemaker on 18-11-26.
//
#ifndef CHAMELEON_PARTICIPANT_HPP
#define CHAMELEON_PARTICIPANT_HPP

// C++ 11 dependencies
#include <iostream>
#include <unordered_map>

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

// protobuf
#include <monitor_info.pb.h>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::unordered_map;

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

    class Slave : public ProtobufProcess<Slave> {
    public:
        explicit Slave():ProcessBase("slave"){

        }

        virtual ~Slave(){

        }

        virtual void initialize() {
            install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);
        }

        void register_feedback(const string& hostname){
            cout<<" receive register feedback from master"<< hostname<<endl;
        }
    };
}




#endif //CHAMELEON_PARTICIPANT_HPP
