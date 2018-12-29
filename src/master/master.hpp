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
#include <unordered_map>
#include <set>
#include <memory>

#include <glog/logging.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/gtest.hpp>
#include <stout/json.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>

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
#include "resources.hpp"
#include "allocator.hpp"
#include "metrics.hpp"
#include "protobuf_utils.hpp"

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

using namespace mesos;

namespace chameleon {


    class Master : public ProtobufProcess<Master> {

    public:

        explicit Master() : ProcessBase("master") {
            msp_spark_slave = make_shared<UPID>(UPID(test_slave_UPID));
            msp_spark_master = make_shared<UPID>(UPID(test_master_UPID));
        }

        virtual ~Master() {

        }

        virtual void initialize();

        struct Framework;

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
         * @param call
         * @param spark run on chameleon
         * Author  : weiguow
         * Date    : 2018-12-28
         * */

        Option<process::UPID> pid;

        struct Framework {
            enum State {
                // Framework has never connected to this master. This implies the
                // master failed over and the framework has not yet re-registered,
                // but some framework state has been recovered from re-registering
                // agents that are running tasks for the framework.
                        RECOVERED,

                // Framework was previously connected to this master. A framework
                // becomes disconnected when there is a socket error.
                        DISCONNECTED,

                // The framework is connected but not active.
                        INACTIVE,

                // Framework is connected and eligible to receive offers. No
                // offers will be made to frameworks that are not active.
                        ACTIVE
            };

            Framework(Master *const master,
                      const FrameworkInfo &info,
                      const process::UPID &_pid,
                      const process::Time &time = process::Clock::now())
                    : Framework(master, info, ACTIVE, time)
                    {
                pid = _pid;
            }

            ~Framework() {}

            // Sends a message to the connected framework.
            template<typename Message>
            void send(const Message &message) {
                CHECK_SOME(pid);
                master->send(pid.get(), message);
            }

            const FrameworkID id() const { return info.id(); }

            // Update fields in 'info' using those in 'newInfo'. Currently this
            // only updates `role`/`roles`, 'name', 'failover_timeout', 'hostname',
            // 'webui_url', 'capabilities', and 'labels'.
            void update(const FrameworkInfo &newInfo) {
                // We only merge 'info' from the same framework 'id'.
                CHECK_EQ(info.id(), newInfo.id());

                // Save the old list of roles for later.
                std::set<std::string> oldRoles = roles;

                // TODO(jmlvanre): Merge other fields as per design doc in
                // MESOS-703.

                info.clear_role();
                info.clear_roles();

                if (newInfo.has_role()) {
                    info.set_role(newInfo.role());
                }

                if (newInfo.roles_size() > 0) {
                    info.mutable_roles()->CopyFrom(newInfo.roles());
                }

                roles = mesos::internal::protobuf::framework::getRoles(newInfo);

                info.set_name(newInfo.name());

                if (newInfo.has_failover_timeout()) {
                    info.set_failover_timeout(newInfo.failover_timeout());
                } else {
                    info.clear_failover_timeout();
                }

                if (newInfo.has_hostname()) {
                    info.set_hostname(newInfo.hostname());
                } else {
                    info.clear_hostname();
                }


                if (newInfo.has_labels()) {
                    info.mutable_labels()->CopyFrom(newInfo.labels());
                } else {
                    info.clear_labels();
                }

                const std::set<std::string> &newRoles = roles;

                const std::set<std::string> addedRoles = [&]() {
                    std::set<std::string> result = newRoles;
                    foreach (const std::string &role, oldRoles) {
                        result.erase(role);
                    }
                    return result;
                }();

            }

            bool active() const { return state == ACTIVE; }

            bool connected() const { return state == ACTIVE || state == INACTIVE; }

            bool recovered() const { return state == RECOVERED; }

            bool isTrackedUnderRole(const std::string &role) const;


            Master *const master;

            FrameworkInfo info;

            std::set<std::string> roles;

            Option<process::UPID> pid;

            State state;

            hashmap<TaskID, Task *> tasks;

            hashset<Offer *> offers; // Active offers for framework.

        private:
            Framework(Master *const _master,
                      const FrameworkInfo &_info,
                      State state,
                      const process::Time &time)
                    : master(_master),
                      info(_info),
                      roles(mesos::internal::protobuf::framework::getRoles(_info)),
                      state(state) {
            }

            Framework(const Framework &);              // No copying.
            Framework &operator=(const Framework &); // No assigning.
        };

        Framework* getFramework(const FrameworkID& frameworkId) const;

        void receive(const process::UPID &from,
                     const mesos::scheduler::Call &call);

        void subscribe(const process::UPID &from,
                       const mesos::scheduler::Call::Subscribe &subscribe);

        void accept(const Framework& framework, mesos::scheduler::Call::Accept accept);

        mesos::Offer *getOffer(const mesos::OfferID &offerId) const;


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


        /**
         * @param spark run on chameleon
         * Author  : weiguow
         * Date    : 2018-12-29
         * */
        std::shared_ptr<mesos::internal::master::Metrics> metrics;
        hashmap<mesos::OfferID, mesos::Offer *> offers;

        void dispatch_offer(const UPID &from);

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
    };
}


#endif //CHAMELEON_MASTER_HPP
