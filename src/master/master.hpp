/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */
#ifndef CHAMELEON_MASTER_HPP
#define CHAMELEON_MASTER_HPP
// C++ 11 dependencies
#include <iostream>
#include <vector>
#include <set>
#include <memory>
#include <hash_map>

#include <glog/logging.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/gtest.hpp>
#include <stout/json.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>
#include <stout/hashmap.hpp>
#include <stout/uuid.hpp>

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
#include <job.pb.h>
#include <runtime_resource.pb.h>
#include <cluster_operation.pb.h>
#include <mesos.pb.h>
#include <scheduler.pb.h>
#include <messages.pb.h>

// chameleon headers
#include <configuration_glog.hpp>
#include <chameleon_string.hpp>
#include "scheduler.hpp"

using std::string;
using std::set;
using std::vector;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;
using std::list;

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

//    struct Slave;
    struct Framework;

    class Master;
    /*
     * Struct : Slave
     * Author : weiguow
     * Date   : 2018-12-31
     * */
//    struct Slave {
//        Slave(Master* const master,
//              const mesos::SlaveInfo &_info,
//              const process::UPID &_pid);
//
//        ~Slave() {}
//
//        Master* master;
//        const mesos::SlaveID id;
//        const mesos::SlaveInfo info;
//        process::UPID pid;
//
//    private:
////        Slave(const Slave &);
//        Slave &operator=(const Slave &);
//
//    };

    /*
     * Struct : Framework
     * Author : weiguow
     * Date   : 2018-12-31
     * */
//    struct Framework {
//    public:
//        enum State {
//            RECOVERED,
//            DISCONNECTED,
//            INACTIVE,
//            ACTIVE
//        };
//
//        Framework(Master *const master, const mesos::FrameworkInfo &info, const process::UPID &_pid)
//                : Framework(master, info, ACTIVE) { pid = _pid; }
//
//        ~Framework() {}
//
//        mesos::FrameworkInfo info;
//
//        Master *master;
//        Option<process::UPID> pid;
//        State state;
//
//        const mesos::FrameworkID id() const { return info.id(); }
//
//    private:
////        Framework(Master *const _master,
////                  const mesos::FrameworkInfo &_info,
////                  State state)
////                : master(_master),
////                  info(_info),
////                  state(state) {}
////
////        Framework(const Framework &);
//
//        Framework &operator=(const Framework &);
//    };

    class Master : public ProtobufProcess<Master> {

    public:
        friend struct Framework;
//        friend struct Slave;

        explicit Master() : ProcessBase("master") {
            msp_spark_slave = make_shared<UPID>(UPID(test_slave_UPID));
            msp_spark_master = make_shared<UPID>(UPID(test_master_UPID));
        }

        virtual ~Master() {}

        virtual void initialize();

        void register_participant(const string &hostname);

        /**
         * get a hardware resource message from a slave, usually happens when a slave registered at the first time.
         * @param from slave UPID
         * @param hardware_resources_message
         */
        void update_hardware_resources(const UPID &from, const HardwareResourcesMessage &hardware_resources_message);

        /**
         * a submitter submits a job to run
         * @param from
         * @param job_message
         */
        void job_submited(const UPID &from, const JobMessage &job_message);

        /**
         * get a heartbeat message from a slave. The heartbeat message contains the runtime resource usage statistics of the slave.
         * @param slave
         * @param runtime_resouces_message represents the runtime resource usage statistics for the slave
         */
        void received_heartbeat(const UPID &slave, const RuntimeResourcesMessage &runtime_resouces_message);

        /**
         * Struct      : Frameworks
         * Description : set hashmap
         * Author      : weiguow
         * Date        : 2019-1-2
         * */
//        struct Frameworks {
//            ~Frameworks() {}
//            hashmap<mesos::FrameworkID, Framework*> registered;
//        } frameworks;

        void Offer(const UPID &from);

        void receive(const process::UPID &from,
                     const mesos::scheduler::Call &call);

        void subscribe(const process::UPID &from,
                       const mesos::scheduler::Call::Subscribe &subscribe);

        void accept(const UPID &from, mesos::scheduler::Call::Accept accept);

        void statusUpdate(mesos::internal::StatusUpdate update, const UPID& pid);

        void statusUpdateAcknowledgement(
                const UPID& from,
                const mesos::SlaveID& slaveId,
                const mesos::FrameworkID& frameworkId,
                const mesos::TaskID& taskId,
                const string& uuid);

        void acknowledge(const mesos::scheduler::Call::Acknowledge& acknowledge);

    private:
        unordered_map<UPID, ParticipantInfo> m_participants;
        unordered_map<string, JSON::Object> m_hardware_resources;
        set<string> m_alive_slaves;

        unordered_map<string, JSON::Object> m_runtime_resources;
        unordered_map<string, RuntimeResourcesMessage> m_proto_runtime_resources;
//        unordered_map<string,HardwareResource> m_topology_resources;

        const string test_slave_UPID = "slave@172.20.110.79:6061";
        const string test_master_UPID = "slave@172.20.110.228:6061";
        shared_ptr<UPID> msp_spark_slave;
        shared_ptr<UPID> msp_spark_master;

        mesos::FrameworkInfo  m_frameworkInfo;
        mesos::FrameworkID m_frameworkID;
        UPID m_frameworkPID;
        string m_slavePID;

        //void Master::handle_accept_call(mesos::scheduler::Call::Accept accept);
        //hashmap<mesos::OfferID, mesos::Offer*> offers;
        //Framework* getFramework(const mesos::FrameworkID& frameworkId) const;
        //mesos::Offer* getOffer(const mesos::OfferID& offerId) const;

        /**
         * a simple algorithm to find a slave which has the least usage rate of cpu and memory combination
         * ( the formula is: combination =  cpu used rate * 50 + memory used rate * 50 )
         * @return the slave ip or an Error if we have no slave
         */
        Try<string> find_min_cpu_and_memory_rates();

        /**
         * get a ReplyShutdownMessage from the slave which belongs to the administration of the current master had shutdown.
         * @param ip  slave.ip
         */
        void received_reply_shutdown_message(const string &ip, const bool &is_shutdown);

        void handle_accept_call(mesos::scheduler::Call::Accept accept);
    };
}


#endif //CHAMELEON_MASTER_HPP
