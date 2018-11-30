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
#include <hardware_resource.pb.h>

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
<<<<<<< HEAD
        explicit Slave():ProcessBase("slave"){}

        virtual ~Slave(){}
=======
        explicit Slave():ProcessBase("slave"){
            msp_resource_collector = make_shared<ResourceCollector>(ResourceCollector());
            msp_resource_collector->collect_hardware_resources();
        }

        virtual ~Slave(){

        }
>>>>>>> 38012e84f8897d7be4e7226e613fb97f46bbc9e0

<<<<<<< HEAD
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
=======
        virtual void initialize();

        void register_feedback(const string& hostname);
>>>>>>> 5f5f6ce08eb49160d2cee4ecd9f22f73ea2469b9

<<<<<<< HEAD
=======
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

>>>>>>> 38012e84f8897d7be4e7226e613fb97f46bbc9e0

    };
}




#endif //CHAMELEON_PARTICIPANT_HPP
