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
#include <process/subprocess.hpp>

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
using process::Subprocess;

namespace chameleon {
    namespace master {
        class Framework;
        class Master;
        class Slave {
        public:
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
            RuntimeResourcesMessage m_runtimeinfo;
            HardwareResourcesMessage m_hardwareinfo;
            const string m_uid;
            process::UPID m_pid;
            const string m_hostname;

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
                m_uuid = UUID::random().toString();

                m_next_framework_id = 0;
                m_next_offer_id = 0;
                m_next_slave_id = 0;

                m_masterinfo.set_id(m_uuid);
                m_masterinfo.set_pid(self());

                m_masterinfo.set_ip(self().address.ip.in().get().s_addr);
                m_masterinfo.set_port(self().address.port);

                m_masterinfo.mutable_address()->set_ip(stringify(self().address.ip));
                m_masterinfo.mutable_address()->set_port(self().address.port);
                m_masterinfo.mutable_address()->set_hostname(stringify(self().address.ip));

                m_master_cwd = os::getcwd();
            }

            virtual ~Master() {}

            virtual void initialize();


            void receive(const process::UPID &from, const mesos::scheduler::Call &call);

            void subscribe(const process::UPID &from, const mesos::scheduler::Call::Subscribe &subscribe);

            void offer(const mesos::FrameworkID &frameworkId);

            void accept(Framework *framework, mesos::scheduler::Call::Accept accept);

            void teardown_framework(Framework *framework);

            void decline_framework(Framework *framework, const mesos::scheduler::Call::Decline &decline);

            void shutdown_slave_executor(Framework *framework, const mesos::scheduler::Call::Shutdown &shutdown);

            void status_update(mesos::internal::StatusUpdate update, const UPID &pid);

            void status_update_acknowledgement(const UPID &from, const mesos::SlaveID &slaveId,
                    const mesos::FrameworkID &frameworkId, const mesos::TaskID &taskId, const string &uuid);

            void acknowledge(Framework *framework, const mesos::scheduler::Call::Acknowledge &acknowledge);

            Slave* find_slave_to_run();

            void add_slave(Slave *slave);

            void add_framework(Framework *framework);

            void remove_framework(Framework *framework);

            mesos::FrameworkID new_framework_id();

            mesos::OfferID new_offer_id();

            Slave *get_slave(const string uid);

            mesos::Offer* get_offer(const mesos::OfferID &offerid);

            Framework *get_framework(const mesos::FrameworkID &kFrameworkId);

            /** save slaveinfo-weiguow-2019-2-24*/
            struct Slaves {
                hashset<process::UPID> registering;

                struct {
                    bool contains(const string KUid) const {
                        return m_uids.contains(KUid);
                    }

                    bool contains(const process::UPID &KPid) const {
                        return m_pids.contains(KPid);
                    }

                    Slave *get(const string kUid) const {
                        return m_uids.get(kUid).getOrElse(nullptr);
                    }

                    Slave *get(const process::UPID &KPid) const {
                        return m_pids.get(KPid).getOrElse(nullptr);
                    }

                    void put(Slave *slave) {
                        CHECK_NOTNULL(slave);
                        m_uids[slave->m_uid] = slave;
                        m_pids[slave->m_pid] = slave;
                    }

                    size_t size() const { return m_uids.size(); }

                    typedef hashmap<string, Slave *>::iterator iterator;
                    typedef hashmap<string, Slave *>::const_iterator const_iterator;

                    iterator begin() { return m_uids.begin(); }

                    iterator end() { return m_uids.end(); }

                    const_iterator begin() const { return m_uids.begin(); }

                    const_iterator end() const { return m_uids.end(); }

                public:
                    hashmap<string, Slave *> m_uids;
                    hashmap<process::UPID, Slave *> m_pids;
                } registered;
            } slaves;

            /**
         * save Frameworkinfo-weiguow-2019-2-22
         * */
            struct Frameworks {
                hashmap<string, Framework *> registered;
            } frameworks;


            void register_participant(const string &hostname);

            /**
             * get a hardware resource message from a slave, usually happens when a slave registered at the first time.
             * @param from slave UPID
             * @param hardware_resources_message
             */
            void update_hardware_resources(const UPID &from,
                    const HardwareResourcesMessage &hardware_resources_message);

            /**
             * get a heartbeat message from a slave. The heartbeat message contains the runtime resource usage statistics of the slave.
             * @param slave
             * @param runtime_resouces_message represents the runtime resource usage statistics for the slave
             */
            void received_heartbeat(const UPID &from, const RuntimeResourcesMessage &runtime_resouces_message);

            void get_select_master(const UPID &from, const string &message);

            void get_slave_infos(const UPID &from, const string &message);

            // super_master related
            void set_super_master_path(const string &path);

            const string get_cwd() const;

            void set_webui_path(const string& path);

            const string get_web_ui() const;

            void get_select_master(const UPID& from, const string& message);

            void get_slave_infos(const UPID& from, const string& message);
            // super_master related
            void set_super_master_path(const string& path);

        private:

            string m_uuid;
            // the absolute path of the directory where the master executable exists.
            string m_master_cwd;

            string m_webui_path;

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

            // super_master_related
            bool is_passive;

            /**
             * get a ReplyShutdownMessage from the slave which belongs to the administration of the current master had shutdown.
             * @param ip  slave.ip
             */
            void received_reply_shutdown_message(const string &ip, const bool &is_shutdown);

            // super_master related
            string m_super_master_path;

            void super_master_control(const UPID &super_master,
                    const SuperMasterControlMessage &super_master_control_message);

            void received_registered_message_from_super_master(const UPID &super_master,
                    const AcceptRegisteredMessage &message);

            void received_terminating_master_message(const UPID &super_master,
                    const TerminatingMasterMessage &message);


            hashmap<string, mesos::Offer*> offers;
            hashmap<UPID, RuntimeResourcesMessage> m_slave_usage;
            mesos::MasterInfo m_masterinfo;

            int64_t m_next_framework_id;
            int64_t m_next_offer_id;
            int64_t m_next_slave_id;

            process::UPID m_slave_pid;
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
                      const mesos::FrameworkInfo &_info,
                      const process::UPID &_pid,
                      const process::Time &time = process::Clock::now()
            ) : Framework(master, _info, ACTIVE, time) {
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

        inline std::ostream &operator<<(std::ostream &stream, const mesos::TaskState &state) {
            return stream << TaskState_Name(state);
        }

        inline std::ostream &operator<<(std::ostream &stream, const mesos::scheduler::Call::Type &type) {
            return stream << mesos::scheduler::Call::Type_Name(type);
        }

        inline std::ostream &operator<<(std::ostream &stream, const mesos::scheduler::Event::Type &type) {
            return stream << mesos::scheduler::Event::Type_Name(type);
        }


    }
}


#endif //CHAMELEON_MASTER_HPP
