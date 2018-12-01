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
        UPID slave;

        explicit Master() : ProcessBase("master") {}

        virtual ~Master(){

        }

        virtual void initialize() {
            install<ParticipantInfo>(&Master::register_participant, &ParticipantInfo::hostname);

            install<HardwareResourcesMessage>(&Master::update_hardware_resources);
            // http://172.20.110.228:5050/master/post-test
            route(
                    "/HardwareResources",
                    "get the topology resources of the whole topology",
                    [](Request request) {
                        string request_method = request.method;
                        std::cout<<request_method <<std::endl;
                        string& tpath = request.url.path;
                        std::cout<<tpath<<std::endl;
                        int param_size = request.url.query.size();
                        std::cout<< param_size<<std::endl;
                        for(string key: request.url.query.keys()){
                            std::cout<<"key:"<<key<<std::endl;
                            std::cout<<"value:"<<request.url.query[key]<<std::endl;
                        }

//                int a = numify<int>(request["a"]).get();
//                int b = numify<int>(request["b"]).get();
                        string body_str = request.body;
                        cout<<body_str<<endl;
                        Option<Pipe::Reader> pipe_reader = request.reader;
                        if(pipe_reader.isSome()){
                            Pipe::Reader reader = pipe_reader.get();
                            Future<string> res = reader.readAll();
                            if(res.isReady()){
                                cout<<"pipe reader content"<<endl;
                                cout<<res.get()<<endl;
                            }
                        }
                        int a = 3;
                        int b = 4;
                        std::ostringstream result;
                        result << "{ \"result\": " <<"\"" <<request_method+tpath <<"\"" << "}";
                        std::cout<<result.str()<<std::endl;
                        JSON::Value body = JSON::parse(result.str()).get();
                        return OK(body);
                    });

//     install("stop", &MyProcess::stop);
            install("stop", [=](const UPID &from, const string &body) {
                terminate(self());
            });

        }


        void register_participant(const string& hostname){
            cout<<"master receive register message from "<< hostname<<endl;
        }

        void update_hardware_resources(const UPID& from, const HardwareResourcesMessage& hardware_resources_message){
            std::cout<<"enter update_hardware_resources"<<std::endl;
            JSON::Object object = JSON::protobuf(hardware_resources_message);
            std::cout<<stringify(object)<<std::endl;

        }



    private:
        unordered_map<UPID,ParticipantInfo> m_participants;
//        unordered_map<string,HardwareResource> m_topology_resources;
    };


}




#endif //CHAMELEON_MONITOR_HPP
