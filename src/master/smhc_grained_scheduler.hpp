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

    class SMHCGrainedScheduler : public SchedulerInterface {
    public:
        explicit SMHCGrainedScheduler() : SchedulerInterface("SMHCGrained") {
        }

        virtual ~SMHCGrainedScheduler() {

        }


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

            int a, c, d, e;
            a = c = d = e = 0;
            double b = 0;

            double F = 0;

            int x_total = 0;
            double y_total = 0;

            LOG(INFO) << "smhc-grained scheduling";

             for (auto it = m_slave_objects.begin(); it != m_slave_objects.end(); it++) {
                  shared_ptr<SlaveObject> slave = it->second;
                 x_total = slave->m_hardware_resources.cpu_collection().cpu_quantity() + x_total ;
                 for (auto i = slave->m_hardware_resources.mem_collection().mem_infos().begin();
                      i != slave->m_hardware_resources.mem_collection().mem_infos().end();
                      i++){
                  vector<string> vec = strings::split(i->size(), "MB");
                  y_total = strtod(vec.front().data(), NULL) + y_total;
              }
            }

            LOG(INFO) << "Have " << x_total << " CPU Cores && " << y_total << " Memory ";

            double result = 0;
            shared_ptr<SlaveObject> result_slave;
//            result_slave = make_shared<SlaveObject>();

            for (auto it = m_slave_objects.begin(); it != m_slave_objects.end(); it++) {

                double x_slave = 0;
                double y_slave = 0;
                double rate = 0.6;

                shared_ptr<SlaveObject> slave = it->second;

                const MemInfo &memInfo = slave->m_hardware_resources.mem_collection().mem_infos(0);
                const CPUInfo &cpuInfo = slave->m_hardware_resources.cpu_collection().cpu_infos(0);
                const DiskInfo &diskInfo = slave->m_hardware_resources.disk_collection().disk_infos(0);

                string m_speed = diskInfo.disk_speed();
                vector<string> vec_mem = strings::split(m_speed, " ");
                d = stoi(vec_mem[0]);                     // memory speed
                if (d < 2000) {
                    d = 1;
                } else if (d >= 2000) {
                    d = 2;
                }

                string m_modal = cpuInfo.modelname();     //Intel(R) Core(TM) i7-2700K CPU @ 3.50GHz
                vector<string> vec_modul = strings::split(m_modal, " ");
                vector<string> vec2_modul = strings::split(vec_modul[3], "-");
                string string_a = vec2_modul[0];
                if (string_a == "i3") {
                    a = 1;
                } else if (string_a == "i5") {
                    a = 2;
                } else if (string_a == "i7") {
                    a = 3;
                } else if (m_modal.find("E5") != string::npos) {
                    a = 20;
                }


                b = cpuInfo.cpumhz();                       // cpu clock speed
                if ((int(b / 1000)) >= 0 && (int(b / 1000)) < 2) {
                    b = 1;
                } else if ((int(b / 1000)) >= 2 && (int(b / 1000)) < 3) {
                    b = 2;
                } else if ((int(b / 1000)) >= 3) {
                    b = 3;
                }

                string m_cpucache = cpuInfo.l3cache();       //l3 cache
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
                if (e < 100) {
                    e = 1;
                } else if (e >= 100) {
                    e = 2;
                }
                double slave_memory = 0;
                for (auto i = slave->m_hardware_resources.mem_collection().mem_infos().begin();
                      i != slave->m_hardware_resources.mem_collection().mem_infos().end(); i++){
                   vector<string> vec_slave = strings::split(i->size(), "MB");
                   slave_memory = strtod(vec_slave.front().data(), NULL) + slave_memory;
                }

                x_slave = slave->m_available_cpus / x_total;
                y_slave = slave_memory / y_total;

                LOG(INFO) << "THis slave have " << slave->m_available_cpus << " CPU rate is " << x_slave
                          << " && have " << memInfo.size().data() << " Memory rate is " << y_slave;

                LOG(INFO) << "Result_slave is " << slave->m_ip;

                F = rate * (a + b + c) * x_slave + (1 - rate) * d * y_slave + e;

                if (F > result) {
                    result = F;
                    result_slave = slave;
                }
            }
            mesos::OfferID offer_id = new_offer_id();
            mesos::Offer *offer = result_slave->construct_a_offer(offer_id.value(), framework_id);
            resource_offers_message.add_offers()->MergeFrom(*offer);
            LOG(INFO) << offer->slave_id().value();
            m_offers[offer->slave_id().value()] = offer->id().value();
            resource_offers_message.add_pids(result_slave->m_upid_str);
        }

    };
};


#endif //CHAMELEON_SMHC_GRAINED_SCHEDULER_HPP
