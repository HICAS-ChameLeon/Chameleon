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
#include <stout/check.hpp>

// libprocess dependencies
#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/delay.hpp>

// protobuf
#include <participant_info.pb.h>
#include <hardware_resource.pb.h>
#include <job.pb.h>
#include <runtime_resource.pb.h>
#include <cluster_operation.pb.h>
#include <mesos.pb.h>
#include <scheduler.pb.h>
#include <messages.pb.h>
#include <super_master_related.pb.h>
#include <slave_related.pb.h>

// chameleon headers
#include <configuration_glog.hpp>
#include <chameleon_string.hpp>
#include <chameleon_os.hpp>
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

    class Framework;

//    class Slave;

    class Master;

    class Master : public ProtobufProcess<Master> {

    public:
        friend class Framework;

        friend class Slave;

        explicit Master() : ProcessBase("master") {
            msp_spark_slave = make_shared<UPID>(UPID(test_slave_UPID));
            msp_spark_master = make_shared<UPID>(UPID(test_master_UPID));
            m_state = INITIALIZING;

            m_masterInfo.set_id(UUID::random().toString());
            m_masterInfo.set_ip(self().address.ip.in().get().s_addr);
            m_masterInfo.set_port(self().address.port);
            m_masterInfo.set_pid(self());

            string hostname;
            hostname = stringify(self().address.ip);

            m_masterInfo.mutable_address()->set_ip(stringify(self().address.ip));
            m_masterInfo.mutable_address()->set_port(self().address.port);
            m_masterInfo.mutable_address()->set_hostname(hostname);
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
           *change framework protobuf to JSON
           * @param from slave UPID
           * @param hardware_resources_message
           */
        void change_frameworks(const UPID &from, const mesos::FrameworkInfo &frameworkInfo);

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


        mesos::SlaveID newSlaveId();
        mesos::FrameworkID newFrameworkId();
        mesos::OfferID newOfferId();

        Framework* getFramework(const mesos::FrameworkID& frameworkId);

//        struct Slaves {
//            struct {
//                Slave *get(const mesos::SlaveID &slaveId) const {
//                    return ids.get(slaveId).getOrElse(nullptr);
//                }
//
//                Slave *get(const process::UPID &pid) const {
//                    return pids.get(pid).getOrElse(nullptr);
//                }
//
//                void put(Slave* slave)
//                {
//                    CHECK_NOTNULL(slave);
//                    ids[slave->id] = slave;
//                    pids[slave->pid] = slave;
//                }
//
//            private:
//                unordered_map<string, Slave *> ids;
//                unordered_map<process::UPID, Slave *> pids;
//            } registered;
//
//
//        } slaves;

        /**
         * Struct      : Frameworks
         * Description : Use this struct to save Frameworkinfo
         * Author      : weiguow
         * Date        : 2019-2-22
         * */
        struct Frameworks {
            Frameworks() {}
            hashmap<string, Framework *> registered;
        } frameworks;

        void Offer(const mesos::FrameworkID &frameworkId);

        void receive(
                const process::UPID &from,
                const mesos::scheduler::Call &call);

        void subscribe(
                const process::UPID &from,
                const mesos::scheduler::Call::Subscribe &subscribe);

        void accept(Framework* framework, mesos::scheduler::Call::Accept accept);

        void statusUpdate(mesos::internal::StatusUpdate update, const UPID &pid);

        void statusUpdateAcknowledgement(
                const UPID &from,
                const mesos::SlaveID &slaveId,
                const mesos::FrameworkID &frameworkId,
                const mesos::TaskID &taskId,
                const string &uuid);

        void acknowledge(Framework* framework, const mesos::scheduler::Call::Acknowledge &acknowledge);

        void addFramework(Framework *framework);


    private:

        string m_uuid;

        // master states.
        enum {
            REGISTERING, // is registering from a super_master
            INITIALIZING,
            RUNNING
        } m_state;

        unordered_map<UPID, ParticipantInfo> m_participants;
        unordered_map<string, JSON::Object> m_hardware_resources;
        unordered_map<string, HardwareResourcesMessage> m_proto_hardware_resources;
        set<string> m_alive_slaves;

        unordered_map<string, JSON::Object> m_runtime_resources;
        unordered_map<string, RuntimeResourcesMessage> m_proto_runtime_resources;
//        unordered_map<string,HardwareResource> m_topology_resources;

        const string test_slave_UPID = "slave@172.20.110.69:6061";
        const string test_master_UPID = "slave@172.20.110.228:6061";
        shared_ptr<UPID> msp_spark_slave;
        shared_ptr<UPID> msp_spark_master;

        string m_slavePID;

        int64_t nextFrameworkId;
        int64_t nextOfferId;
        int64_t nextSlaveId;

        mesos::MasterInfo m_masterInfo;

        // super_master_related
        bool is_passive;

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

        mesos::Offer *create_a_offer();

        // super_master related
        void
        super_master_control(const UPID &super_master, const SuperMasterControlMessage &super_master_control_message);

        void
        received_registered_message_from_super_master(const UPID &super_master, const AcceptRegisteredMessage &message);

        void received_terminating_master_message(const UPID &super_master, const TerminatingMasterMessage &message);
    };

//    class Slave {
//    public:
//        Slave(Master
//              *const _master,
//              const mesos::SlaveInfo &_info,
//              const process::UPID &_pid,
//              const std::vector<mesos::ExecutorInfo> &executorInfos =
//              std::vector<mesos::ExecutorInfo>(),
//              const std::vector<mesos::Task> &tasks =
//              std::vector<mesos::Task>()
//        );
//
//        ~Slave();
//
//        Master *const master;
//        const mesos::SlaveID id;
//        const mesos::SlaveInfo info;
//
//        process::UPID pid;
//
////        hashmap<FrameworkID, hashmap<ExecutorID, ExecutorInfo>> executors;
////        hashmap<FrameworkID, hashmap<TaskID, Task*>> tasks;
////        hashset<Offer*> offers;
//
//    private:
//        Slave(Master *const _master,
//              const mesos::SlaveInfo &_info,
//              const UPID &_pid,
//              const vector<mesos::ExecutorInfo> &executorInfos,
//              const vector<mesos::Task> &tasks)
//                : master(_master),
//                  id(_info.id()),
//                  info(_info) {}
//
//
//        Slave(const Slave &);
//
//        Slave &operator=(const Slave &);
//
//    };

    class Framework {
    public:
        enum State {
            //re-registered
                    RECOVERED,

            //Framwork not connected
                    DISCONNECTED,

            //Framework connected, but doesn't have offer
                    INACTIVE,

            //Framework connected, has offer
                    ACTIVE
        };

        Framework(Master *const master,
                  const mesos::FrameworkInfo &info,
                  const process::UPID &_pid
        ) : Framework(master, info, ACTIVE) {
            pid = _pid;
        }

        ~Framework() {}

        template<typename Message>
        void send(const Message &message) {
            if (!connected()) {
                LOG(WARNING) << "Master attempted to send message to disconnected"
                             << " framework " << this->state;
            } else {
                master->send(pid.get(), message);
            }
        }

        bool active() const { return state == ACTIVE; }

        bool connected() const { return state == ACTIVE || state == INACTIVE; }

        bool recovered() const { return state == RECOVERED; }

        const mesos::FrameworkID id() const { return info.id(); }

        Master *const master;
        mesos::FrameworkInfo info;
        Option<process::UPID> pid;
        State state;

    private:
        Framework(Master *const _master,
                  const mesos::FrameworkInfo &_info,
                  State state
        )
                : master(_master),
                  info(_info),
                  state(state) {}

        Framework(const Framework &);

        Framework &operator=(const Framework &);

    };

//    inline std::ostream &operator<<(std::ostream &stream, const Slave &slave) {
//        return stream << << " at " << slave.pid
//                      << " (" << slave.info.hostname() << ")";
//    }

    inline std::ostream &operator<<(std::ostream &stream, const Framework &framework) {
        stream << framework.id().value() << " (" << framework.info.name() << ")";
        if (framework.pid.isSome()) {
            stream << " at " << framework.pid.get();
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const mesos::TaskState &state);

}


#endif //CHAMELEON_MASTER_HPP
