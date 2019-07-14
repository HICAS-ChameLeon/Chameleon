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
#include <chameleon_SMHC.hpp>

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

        virtual ~SMHCGrainedScheduler(){

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
                              const unordered_map<string, shared_ptr<SlaveObject>> &m_slave_objects) override ;

    };
};


#endif //CHAMELEON_SMHC_GRAINED_SCHEDULER_HPP
