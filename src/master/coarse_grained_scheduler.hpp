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
        explicit CoarseGrainedScheduler() : SchedulerInterface("CoarseGrained"), m_round_robin_index(0) {

        }


        virtual ~CoarseGrainedScheduler() {

        }

        /**
       *  coarse-grained scheduling: we do not consider the fine-grained resources heterogeneity of hardware, such as cpu speed.
         *  we will find all the slave objects that can satisfy the framework's coarse-grained resource requirements
         *  轮询调度算法(Round-Robin Scheduling)

　　     轮询调度算法的原理是每一次把来自用户的请求轮流分配给内部中的服务器，从1开始，直到N(内部服务器个数)，然后重新开始循环。
       * @param resource_offers_message
       * @param framework_id
       * @param m_slave_objects
       */
        void construct_offers(mesos::internal::ResourceOffersMessage &resource_offers_message,
                              const mesos::FrameworkID &framework_id,
                              const unordered_map<string, shared_ptr<SlaveObject>> &m_slave_objects) override {
            LOG(INFO) << "coarse-grained scheduling, round-robin algorithm";
            int current_size = m_slave_objects.size();
            auto it = m_slave_objects.begin();

            if (m_round_robin_index == current_size) {
                m_round_robin_index = 0;
                it = m_slave_objects.begin();
            }

            int index = 0;
            while (index < m_round_robin_index) {
                it++;
                index++;
            }
            m_round_robin_index++;

            mesos::OfferID offer_id = new_offer_id();
            shared_ptr<SlaveObject> slave = it->second;
            mesos::Offer *offer = slave->construct_a_offer(offer_id.value(), framework_id);
            resource_offers_message.add_offers()->MergeFrom(*offer);
            LOG(INFO) << offer->slave_id().value();
            m_offers[offer->slave_id().value()] = offer->id().value();
            resource_offers_message.add_pids(slave->m_upid_str);

//            for (auto it = m_slave_objects.begin(); it != m_slave_objects.end(); it++) {
//                mesos::OfferID offer_id = new_offer_id();
//                shared_ptr<SlaveObject> slave = it->second;
//                mesos::Offer *offer = slave->construct_a_offer(offer_id.value(), framework_id);
//                resource_offers_message.add_offers()->MergeFrom(*offer);
//                LOG(INFO) << offer->slave_id().value();
//                m_offers[offer->slave_id().value()] = offer->id().value();
//                resource_offers_message.add_pids(slave->m_upid_str);
//            }
        }

    private:
        int m_round_robin_index;
    };
};

#endif //CHAMELEON_COARSE_GRAINED_SCHEDULING_HPP
