/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */
#ifndef CHAMELEON_MONITOR_HPP
#define CHAMELEON_MONITOR_HPP
// C++ 11 dependencies
#include <iostream>
#include <unordered_map>

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

// chameleon headers
#include <configuration_glog.hpp>

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
        UPID slave;

        explicit Master() : ProcessBase("master") {}

        virtual ~Master(){

        }

        virtual void initialize() {
            // Verify that the version of the library that we linked against is
            // compatible with the version of the headers we compiled against.
            GOOGLE_PROTOBUF_VERIFY_VERSION;

            install<ParticipantInfo>(&Master::register_participant, &ParticipantInfo::hostname);

            install<HardwareResourcesMessage>(&Master::update_hardware_resources);
            // http://172.20.110.228:5050/master/post-test
            route(
                    "/HardwareResources",
                    "get the topology resources of the whole topology",
                    [](Request request) {
                        string request_method = request.method;
                        DLOG(INFO) <<request_method ;
                        string& tpath = request.url.path;
                        DLOG(INFO) <<tpath;
                        int param_size = request.url.query.size();
                        DLOG(INFO) << param_size;
                        for(string key: request.url.query.keys()){
                            DLOG(INFO) <<"key:"<<key;
                            DLOG(INFO) <<"value:"<<request.url.query[key];
                        }

//                int a = numify<int>(request["a"]).get();
//                int b = numify<int>(request["b"]).get();
                        string body_str = request.body;
                        DLOG(INFO) <<body_str;
                        Option<Pipe::Reader> pipe_reader = request.reader;
                        if(pipe_reader.isSome()){
                            Pipe::Reader reader = pipe_reader.get();
                            Future<string> res = reader.readAll();
                            if(res.isReady()){
                                DLOG(INFO) <<"pipe reader content";
                                DLOG(INFO) <<res.get();
                            }
                        }
                        int a = 3;
                        int b = 4;
                        std::ostringstream result;
                        result << "{ \"result\": " <<"\"" <<request_method+tpath <<"\"" << "}";
                        DLOG(INFO) <<result.str();
                        JSON::Value body = JSON::parse(result.str()).get();
                        return OK(body);
                    });

//     install("stop", &MyProcess::stop);
            install("stop", [=](const UPID &from, const string &body) {
                terminate(self());
            });

        }


        void register_participant(const string& hostname){
            DLOG(INFO) <<"master receive register message from "<< hostname;
        }

        void update_hardware_resources(const UPID& from, const HardwareResourcesMessage& hardware_resources_message){
            DLOG(INFO) <<"enter update_hardware_resources";
            JSON::Object object = JSON::protobuf(hardware_resources_message);
            DLOG(INFO) <<stringify(object);

        }



    private:
        unordered_map<UPID,ParticipantInfo> m_participants;
//        unordered_map<string,HardwareResource> m_topology_resources;
    };


}




#endif //CHAMELEON_MONITOR_HPP
