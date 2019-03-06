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
#include <stout/boundedhashmap.hpp>

// libprocess dependencies
#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/delay.hpp>

// protobuf
#include <job.pb.h>
#include <mesos.pb.h>
#include <messages.pb.h>
#include <scheduler.pb.h>
#include <slave_related.pb.h>
#include <participant_info.pb.h>
#include <hardware_resource.pb.h>
#include <runtime_resource.pb.h>
#include <cluster_operation.pb.h>
#include <super_master_related.pb.h>


// chameleon headers
#include <configuration_glog.hpp>
#include <chameleon_string.hpp>
#include <chameleon_os.hpp>
#include <runtime_resources_usage.hpp>

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

namespace master {

    class Framework;

    class Master;

    class Slave {
    public:
//        Slave(Master *const _master,
//              const RuntimeResourcesMessage &_info,
//              const string _id,
//              const string _hostname,
//              const process::UPID &_pid) :
//                master(_master),
//                runtimeInfo(_info),
//                id(_id),
//                hostname(_hostname),
//                pid(_pid) {
//        };


        Slave(Master *const _master,
              const HardwareResourcesMessage &_info,
              const string _uid,
              const string _hostname,
              const process::UPID &_pid) :
                m_master(_master),
                m_hardwareinfo(_info),
                m_uid(_uid),
                m_hostname(_hostname),
                m_pid(_pid) {
        };

        ~Slave();

        Master *const m_master;

        const RuntimeResourcesMessage m_runtimeinfo;
        const HardwareResourcesMessage m_hardwareinfo;

        const string m_uid;
        process::UPID m_pid;

        const string m_hostname;

        hashmap<string, RuntimeResourcesMessage> m_usage;

    private:
        Slave(const Slave &);

        Slave &operator=(const Slave &);
    };

    class Master : public ProtobufProcess<Master> {

    public:
        friend class Framework;
        friend class Slave;

        explicit Master() : ProcessBase("master") {

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
         * get a heartbeat message from a slave. The heartbeat message contains the runtime resource usage statistics of the slave.
         * @param slave
         * @param runtime_resouces_message represents the runtime resource usage statistics for the slave
         */
        void received_heartbeat(const UPID &from, const RuntimeResourcesMessage &runtime_resouces_message);


        mesos::FrameworkID new_framework_id();
        mesos::OfferID new_offer_id();
        const string new_slave_id(const string kUid);

        hashmap<string, mesos::Offer *> offers;

        /**save slaveinfo-weiguow-2019-2-24*/
        struct Slaves {

            hashset<process::UPID> registering;

            struct {
                bool contains(const string KUid) const {
                    return uids.contains(KUid);
                }

                bool contains(const process::UPID &KPid) const {
                    return pids.contains(KPid);
                }

                Slave *get(const string kUid) const {
                    return uids.get(kUid).getOrElse(nullptr);
                }

                Slave *get(const process::UPID &KPid) const {
                    return pids.get(KPid).getOrElse(nullptr);
                }

                void put(Slave *slave) {
                    CHECK_NOTNULL(slave);
                    uids[slave->m_uid] = slave;
                    pids[slave->m_pid] = slave;
                }

                size_t size() const { return uids.size(); }

                typedef hashmap<string, Slave *>::iterator iterator;
                typedef hashmap<string, Slave *>::const_iterator const_iterator;

                iterator begin() { return uids.begin(); }
                iterator end() { return uids.end(); }

                const_iterator begin() const { return uids.begin(); }
                const_iterator end() const { return uids.end(); }

            public:
                hashmap<string, Slave *> uids;
                hashmap<process::UPID, Slave *> pids;
            } registered;

        } slaves;

        /**
         * save Frameworkinfo-weiguow-2019-2-22
         * */
        struct Frameworks {

            hashmap<string, Framework *> registered;

//            BoundedHashMap<string, Framework *> completed;

        } frameworks;

        void Offer(const mesos::FrameworkID &frameworkId);

        void receive(
                const process::UPID &from,
                const mesos::scheduler::Call &call);

        void subscribe(
                const process::UPID &from,
                const mesos::scheduler::Call::Subscribe &subscribe);

        void teardown_framework(Framework *framework);

        void accept(Framework *framework, mesos::scheduler::Call::Accept accept);

        void decline_framework(Framework *framework, const mesos::scheduler::Call::Decline &decline);

        void shutdown_framework(Framework *framework, const mesos::scheduler::Call::Shutdown &shutdown);

        void status_update(mesos::internal::StatusUpdate update, const UPID &pid);

        void status_update_acknowledgement(
                const UPID &from,
                const mesos::SlaveID &kSlaveId,
                const mesos::FrameworkID &kFrameworkId,
                const mesos::TaskID &KTaskId,
                const string &KUuid);

        void acknowledge(Framework *framework, const mesos::scheduler::Call::Acknowledge &kacknowledge);

        void add_slave(Slave *slave);

        Slave* get_slave(const string uid);

        Framework *get_framework(const mesos::FrameworkID &kFrameworkId);

        void add_framework(Framework *framework);

        void remove_framework(Framework *framework);

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
        unordered_map<string, JSON::Object> m_runtime_resources;
        unordered_map<string, RuntimeResourcesMessage> m_proto_runtime_resources;

        set<string> m_alive_slaves;
//        unordered_map<string,HardwareResource> m_topology_resources;

        string m_slavePID;

        int64_t m_next_framework_id;
        int64_t m_next_offer_id;
        int64_t m_next_slave_id;

        mesos::MasterInfo m_masterInfo;

        // super_master_related
        bool is_passive;

        /**
         * a simple algorithm to find a slave which has the least usage rate of cpu and memory combination
         * ( the formula is: combination =  cpu used rate * 50 + memory used rate * 50 )
         * @return the slave ip or an Error if we have no slave
         */
        string find_min_cpu_and_memory_rates();

        /**
         * get a ReplyShutdownMessage from the slave which belongs to the administration of the current master had shutdown.
         * @param ip  slave.ip
         */
        void received_reply_shutdown_message(const string &ip, const bool &is_shutdown);

        mesos::Offer *create_a_offer(const mesos::FrameworkID &frameworkId);

        // super_master related
        void
        super_master_control(const UPID &super_master, const SuperMasterControlMessage &super_master_control_message);

        void
        received_registered_message_from_super_master(const UPID &super_master, const AcceptRegisteredMessage &message);

        void received_terminating_master_message(const UPID &super_master, const TerminatingMasterMessage &message);
    };

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
                    ACTIVE,
        };

        Framework(Master *const master,
                  const mesos::FrameworkInfo &info,
                  const process::UPID &_pid,
                  const process::Time &time = process::Clock::now()
        ) : Framework(master, info, ACTIVE, time) {
            m_pid = _pid;
        }

        ~Framework() {}

        template<typename Message>
        void send(const Message &message) {
            if (!connected()) {
                LOG(WARNING) << "Master attempted to send message to disconnected"
                             << " framework " << this->state;
            } else {
                m_master->send(m_pid.get(), message);
            }
        }

        bool active() const { return state == ACTIVE; }

        bool connected() const { return state == ACTIVE || state == INACTIVE; }

        bool recovered() const { return state == RECOVERED; }

        const mesos::FrameworkID id() const { return m_info.id(); }

        Master *const m_master;
        mesos::FrameworkInfo m_info;
        Option<process::UPID> m_pid;
        State state;

        process::Time m_registered_time;
        process::Time m_unregistered_time;

    private:
        Framework(Master *const _master,
                  const mesos::FrameworkInfo &_info,
                  State state,
                  const process::Time &time
        ) : m_master(_master),
            m_info(_info),
            state(state),
            m_registered_time(time) {}

        Framework(const Framework &);

        Framework &operator=(const Framework &);

    };

    inline std::ostream &operator<<(std::ostream &stream, const Slave &slave) {
        return stream << slave.m_uid << " at " << slave.m_pid
                      << " (" << slave.m_hostname << ")";
    }

    inline std::ostream &operator<<(std::ostream &stream, const Framework &framework) {
        stream << framework.id().value() << " (" << framework.m_info.name() << ")";
        if (framework.m_pid.isSome()) {
            stream << " at " << framework.m_pid.get();
        }
        return stream;
    }



}


#endif //CHAMELEON_MASTER_HPP
