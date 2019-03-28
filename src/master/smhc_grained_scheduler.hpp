//
//* Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
//* Author     ：Jessicallo  sherlock_vip@163.com
//* Date       ：19-3-26
//* Description： LiLeli
//

#ifndef CHAMELEON_SMHC_GRAINED_SCHEDULER_HPP
#define CHAMELEON_SMHC_GRAINED_SCHEDULER_HPP

#include <iostream>
#include <vector>
#include <set>
#include <memory>
#include <unordered_map>

// protobuf
#include <hardware_resource.pb.h>
#include <runtime_resource.pb.h>
#include <cluster_operation.pb.h>
#include <mesos.pb.h>
#include <scheduler.pb.h>
#include <messages.pb.h>

// chameleon headers
#include <chameleon_resources.hpp>
#include <slave_object.hpp>
#include <scheduler_interface.hpp>


using std::string;
using std::set;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::shared_ptr;
using std::make_shared;
using std::list;

namespace chameleon {

    class SMHCGrainedScheduler : public SchedulerInterface{
    public:
        explicit SMHCGrainedScheduler():SchedulerInterface("SMHCGrained"){
        }
        virtual ~SMHCGrainedScheduler(){

        }
        double F ;

        int a ;
        double b ;
        int c ;
        int d ;
        int e ;

        int x = 1;
        double y = 0.6;
        /**
      *  coarse-grained scheduling: we do not consider the fine-grained resources heterogeneity of hardware, such as memory disk.
        *  we will find all the slave objects that can satisfy the framework's wqn-grained resource requirements
      * @param resource_offers_message
      * @param framework_id
      * @param m_slave_objects
      **/

      void construct_offers(mesos::internal::ResourceOffersMessage &resource_offers_message,
                              const mesos::FrameworkID &framework_id,
                              const unordered_map<string, shared_ptr<SlaveObject>> &m_slave_objects) override {

            LOG(INFO)<<"Wqn-grained scheduling";

            for(auto it = m_slave_objects.begin();it != m_slave_objects.end();it ++){
                shared_ptr<SlaveObject> slave = it->second;

                //x = slave->m_hardware_resources.cpu_collection().cpu_quantity();
                string max_size = slave->m_hardware_resources.mem_collection().max_size();

                auto m_mem_collection = slave->m_hardware_resources.mem_collection();
                auto m_mem_collection_info = slave->m_hardware_resources.mem_collection().mem_infos();
                auto m_cpu_collection = slave->m_hardware_resources.cpu_collection();
                auto m_cpu_collection_info = slave->m_hardware_resources.cpu_collection().cpu_infos();
                auto m_disk_collection = slave->m_hardware_resources.disk_collection();
                auto m_disk_collection_info = slave->m_hardware_resources.disk_collection().disk_infos();


                for (int i = 0; i < m_slave_objects.size(); i++) {


                    const MemInfo &memInfo = m_mem_collection.mem_infos(i);
                    const CPUInfo &cpuInfo = m_cpu_collection.cpu_infos(i);
                    const DiskInfo &diskInfo = m_disk_collection.disk_infos(i);

                    string m_speed = memInfo.speed();
                    vector<string> vec_mem = strings::split(m_speed, " ");
                    d = stoi(vec_mem[0]);                     // memory speed
                    if (d < 2000) {
                        d = 1;
                    } else if (d >= 2000) {

                        d = 2;
                    }

                    string m_modal = cpuInfo.modelname();     //Intel(R) Core(TM) i7-2700K CPU @ 3.50GHz
                    vector<string> vec_modul = strings::split(m_modal," ");
                    vector<string> vec2_modul = strings::split(vec_modul[3],"-");
                    string string_a = vec2_modul[0];
                    if (string_a == "i3") {
                        a = 1;
                    } else if (string_a == "i5") {
                        a = 2;
                    } else if (string_a == "i7") {
                        a = 3;
                    }


                    b = cpuInfo.cpumhz();                       // cpu clock speed
                    if ((int(b / 1000)) >= 2 && (int(b / 1000)) < 3) {
                        b = 1;
                    } else if ((int(b / 1000)) >= 3) {
                        b = 2;
                    }

                    string m_cpucache = cpuInfo.l3cache();      //l3 cache
                    vector<string> vec_cpu = strings::split(m_cpucache, "K");
                    c = stoi(vec_mem[0]) / 1000;
                    if (c > 0 && c <= 10) {
                        c = 1;
                    } else if (c > 10 && c <= 20) {
                        c = 2;
                    } else if (c > 20 && c <= 30) {
                        c = 3;
                    } else if (c > 30) {
                        c = 4;
                    }

                    string m_diskspeed = diskInfo.disk_speed();     //Disk speed
                    e = stoi(m_diskspeed);
                    if (e < 100)
                    {
                        e =1;
                    } else if(e>=100){

                        e =2;
                    }

                    LOG(INFO) << a;
                    LOG(INFO) << b;
                    LOG(INFO) << c;
                    LOG(INFO) << d;
                    LOG(INFO) << e;
                    LOG(INFO) << m_diskspeed;
                    LOG(INFO) << memInfo.speed();
                }

                F = (a+b+c)*x +d*y + e;

                mesos::OfferID offer_id = new_offer_id();
                mesos::Offer *offer = slave->construct_a_offer(offer_id.value(), framework_id);
                resource_offers_message.add_offers()->MergeFrom(*offer);
                LOG(INFO) << offer->slave_id().value();
                resource_offers_message.add_pids(slave->m_upid_str);
                LOG(INFO) << F;

            }

        }



    };
};



#endif //CHAMELEON_SMHC_GRAINED_SCHEDULER_HPP
