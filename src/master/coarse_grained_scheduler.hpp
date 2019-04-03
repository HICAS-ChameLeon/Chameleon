//
//* Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
//* Author     ：Lele Li lilelr@163.com
//* Date       ：19-3-20
//* Description： coarse-grained scheduling implementation
//

#ifndef CHAMELEON_COARSE_GRAINED_SCHEDULING_HPP
#define CHAMELEON_COARSE_GRAINED_SCHEDULING_HPP

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

    class CoarseGrainedScheduler : public SchedulerInterface {
    public:
        explicit CoarseGrainedScheduler():SchedulerInterface("CoarseGrained") {

        }


        virtual ~CoarseGrainedScheduler() {

        }

        /**
       *  coarse-grained scheduling: we do not consider the fine-grained resources heterogeneity of hardware, such as cpu speed.
         *  we will find all the slave objects that can satisfy the framework's coarse-grained resource requirements
       * @param resource_offers_message
       * @param framework_id
       * @param m_slave_objects
       */
        void construct_offers(mesos::internal::ResourceOffersMessage &resource_offers_message,
                              const mesos::FrameworkID &framework_id,
                              const unordered_map<string, shared_ptr<SlaveObject>> &m_slave_objects) override {
            LOG(INFO)<<"coarse-grained scheduling ";
            for (auto it = m_slave_objects.begin(); it != m_slave_objects.end(); it++) {
                mesos::OfferID offer_id = new_offer_id();
                shared_ptr<SlaveObject> slave = it->second;
                mesos::Offer *offer = slave->construct_a_offer(offer_id.value(), framework_id);
                resource_offers_message.add_offers()->MergeFrom(*offer);
                LOG(INFO) << offer->slave_id().value();
                m_offers[offer->slave_id().value()] = offer->id().value();
                resource_offers_message.add_pids(slave->m_upid_str);
            }
        }
    };


    class WqnGrainedScheduler : public SchedulerInterface{
    public:
        explicit WqnGrainedScheduler():SchedulerInterface("WqnGrained"){
        }
         virtual ~WqnGrainedScheduler(){

        }

        /**
      *  coarse-grained scheduling: we do not consider the fine-grained resources heterogeneity of hardware, such as memory disk.
        *  we will find all the slave objects that can satisfy the framework's wqn-grained resource requirements
      * @param resource_offers_message
      * @param framework_id
      * @param m_slave_objects
      */

        void construct_offers(mesos::internal::ResourceOffersMessage &resource_offers_message,
                              const mesos::FrameworkID &framework_id,
                              const unordered_map<string, shared_ptr<SlaveObject>> &m_slave_objects) override {

            LOG(INFO)<<"Wqn-grained scheduling";
            for(auto it = m_slave_objects.begin();it != m_slave_objects.end();it ++){
                shared_ptr<SlaveObject> slave = it->second;
                string max_size = slave->m_hardware_resources.mem_collection().max_size();
                LOG(INFO)<<"max size"<<max_size;
                LOG(INFO)<<"m_available_mem:"<<slave->m_available_mem<<"m_available_cpu:"<<slave->m_available_cpus<<"m_availiable_disk:"<<slave->m_available_disk;

                if(slave->m_available_mem > 0 && slave->m_available_mem<5000)
                {
                    mesos::OfferID offer_id = new_offer_id();
                    mesos::Offer *offer = slave->construct_a_offer(offer_id.value(), framework_id);
                    resource_offers_message.add_offers()->MergeFrom(*offer);
                    LOG(INFO) << "wqn slave_id"<<offer->slave_id().value();
                    m_offers[offer->slave_id().value()] = offer->id().value();
                    resource_offers_message.add_pids(slave->m_upid_str);
                }
                else{
                    LOG(INFO)<< "Memory resources exceeding the limit ";
                }


            }

        }



    };
};

#endif //CHAMELEON_COARSE_GRAINED_SCHEDULING_HPP
