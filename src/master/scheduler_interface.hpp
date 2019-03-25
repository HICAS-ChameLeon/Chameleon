////
//* Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
//* Author     ：Lele Li lilelr@163.com
//* Date       ：19-3-20
//* Description：scheduler_interface
////

#ifndef CHAMELEON_SCHEDULER_INTERFACE_HPP
#define CHAMELEON_SCHEDULER_INTERFACE_HPP

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

using std::string;
using std::set;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::shared_ptr;
using std::make_shared;
using std::list;

namespace chameleon {
    class SchedulerInterface {
    public:
        const string m_scheduler_name;

        explicit SchedulerInterface(const string& name_) : m_next_offer_id(0),m_scheduler_name(name_) {

        }

        SchedulerInterface(const SchedulerInterface &) = delete;

        SchedulerInterface &operator=(const SchedulerInterface &) = delete;

        /**
         * different offer strategies need to implement this function to make up the final ResourceOfferMessage.
         * @param resource_offers_message
         * @param framework_id
         * @param m_slave_objects
         */
        virtual void construct_offers(mesos::internal::ResourceOffersMessage &resource_offers_message,
                                      const mesos::FrameworkID &framework_id,
                                      const unordered_map<string, shared_ptr<SlaveObject>> &m_slave_objects) = 0;

        virtual ~SchedulerInterface() {

        }

    protected:
        mesos::OfferID new_offer_id() {
            mesos::OfferID offer_id;
            offer_id.set_value(stringify(m_next_offer_id));
            return offer_id;
        }

        int64_t m_next_offer_id;


    };
}


#endif //CHAMELEON_SCHEDULER_INTERFACE_HPP
