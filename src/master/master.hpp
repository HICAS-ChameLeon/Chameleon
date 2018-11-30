//
// Created by lemaker on 18-11-26.
//

#ifndef CHAMELEON_MONITOR_HPP
#define CHAMELEON_MONITOR_HPP
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
#include <participant_info.pb.h>

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

    class Master : public ProtobufProcess<Master> {

    public:
        explicit Master() : ProcessBase("master") {

        }

        virtual ~Master(){

        }

        virtual void initialize() {
            install<ParticipantInfo>(&Master::register_participant, &ParticipantInfo::hostname);


        }

        void register_participant(const string& hostname){
            cout<<"master receive register message from "<< hostname<<endl;
        }



    private:
        unordered_map<UPID,ParticipantInfo> m_participants;
//        unordered_map<string,HardwareResource> m_topology_resources;
    };


}




#endif //CHAMELEON_MONITOR_HPP
