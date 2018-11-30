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

    class Slave : public ProtobufProcess<Slave> {
    public:
        explicit Slave():ProcessBase("slave"){}

        virtual ~Slave(){}

        virtual void initialize() {
            install<MonitorInfo>(&Slave::register_feedback, &MonitorInfo::hostname);
            
            /**get cpuinfomation
             * */
            
        }

        void get_cpuinfo_request_from_server(const string &address){
            AllCpuInfo ac;
            UPID clientUPID(address);
//        Try<list<CPU>> cpus = weiguo_proc::cpus();

            cpu_collector *cpus;
            list<cpu_collector> cpu_info = cpus->get_cpu_info();
            for(auto i= cpu_info.begin();i!=cpu_info.end();i++){
                ac.add_sci()->set_cpuid(i->getCpuID());
                ac.add_sci()->set_coreid(i->getCoreID());
                ac.add_sci()->set_physicalid(i->getPhysicalID());
                ac.add_sci()->set_cpucores(i->getCpucores());
                ac.add_sci()->set_modelname(i->getModelname());
                ac.add_sci()->set_cpumhz(i->getCpuMHz());
                ac.add_sci()->set_l1dcache(i->getL1dcache());
                ac.add_sci()->set_l1icache(i->getL1icache());
                ac.add_sci()->set_l2cache(i->getL2cache());
                ac.add_sci()->set_l3cache(i->getL3cache());
            }
            send(clientUPID,ac);
        }
        
        void register_feedback(const string& hostname){
            cout<<" receive register feedback from master"<< hostname<<endl;
        }


    };
}




#endif //CHAMELEON_PARTICIPANT_HPP
