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

#include <boost/circular_buffer.hpp>

// stout dependencies
#include <stout/gtest.hpp>
#include <stout/json.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>
#include <stout/os.hpp>
#include <stout/os/pstree.hpp>
#include <stout/boundedhashmap.hpp>

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

    class Master : public ProtobufProcess<Master> {

    public:
        explicit Master() : ProcessBase("master") {
            msp_spark_slave = make_shared<UPID>(UPID(test_slave_UPID));
            msp_spark_master = make_shared<UPID>(UPID(test_master_UPID));
        }

        virtual ~Master() {

        }

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
         * @param call
         * @param spark run on chameleon
         * Author  : weiguow
         * Date    : 2018-12-28
         * */
        void receive(const process::UPID &from,
                     const mesos::scheduler::Call &call);

        void subscribe(const process::UPID &from,
                       const mesos::scheduler::Call::Subscribe &subscribe);

        //void accept(mesos::internal::Framework* framework,mesos::scheduler::Call::Accept accept);


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


    /*
     *
     */
    struct Framework
    {
        enum State
        {
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


        Framework(Master* const master,
                  /*const Flags& masterFlags,*/
                  const mesos::FrameworkInfo& info,
                  const process::UPID& _pid,
                  const process::Time& time = process::Clock::now())
                : Framework(master, info, ACTIVE, time)
        {
            pid = _pid;
        }

        ~Framework() {}

//        mesos::Task* getTask(const mesos::TaskID& taskId)
//        {
//            if (tasks.count(taskId) > 0) {
//                return tasks[taskId];
//            }
//
//            return nullptr;
//        }

        // Sends a message to the connected framework.
        template <typename Message>
        void send(const Message& message)
        {
            if (!connected()) {
                LOG(WARNING) << "Master attempted to send message to disconnected"
                             << " framework ";
            }
            CHECK_SOME(pid);
            master->send(pid.get(), message);

        }

        const mesos::FrameworkID id() const { return info.id(); }

        bool active() const    { return state == ACTIVE; }
        bool connected() const { return state == ACTIVE || state == INACTIVE; }
        bool recovered() const { return state == RECOVERED; }

        bool isTrackedUnderRole(const std::string& role) const;
        void trackUnderRole(const std::string& role);
        void untrackUnderRole(const std::string& role);

        Master* const master;

        mesos::FrameworkInfo info;

        std::set<std::string> roles;

        //protobuf::framework::Capabilities capabilities;

        // Frameworks can either be connected via HTTP or by message passing
        // (scheduler driver). At most one of `http` and `pid` will be set
        // according to the last connection made by the framework; neither
        // field will be set if the framework is in state `RECOVERED`.
        Option<process::UPID> pid;

        State state;

        process::Time registeredTime;
        process::Time reregisteredTime;
        process::Time unregisteredTime;

        // Tasks that have not yet been launched because they are currently
        // being authorized.
//        hashmap<mesos::TaskID, mesos::TaskInfo> pendingTasks;

        // TODO(bmahler): Make this private to enforce that `addTask()` and
        // `removeTask()` are used, and provide a const view into the tasks.
//        hashmap<mesos::TaskID, mesos::Task*> tasks;

        // Tasks launched by this framework that have reached a terminal
        // state and have had all their updates acknowledged. We only keep a
        // fixed-size cache to avoid consuming too much memory. We use
        // boost::circular_buffer rather than BoundedHashMap because there
        // can be multiple completed tasks with the same task ID.
        //
        // NOTE: When an agent is marked unreachable, non-partition-aware
        // tasks are marked TASK_LOST and stored here; partition-aware tasks
        // are marked TASK_UNREACHABLE and stored in `unreachableTasks`.
        boost::circular_buffer<process::Owned<mesos::Task>> completedTasks;

        // Partition-aware tasks running on agents that have been marked
        // unreachable. We only keep a fixed-size cache to avoid consuming
        // too much memory.
//        BoundedHashMap<mesos::TaskID, process::Owned<mesos::Task>> unreachableTasks;

//        hashset<mesos::Offer*> offers; // Active offers for framework.

        hashset<mesos::InverseOffer*> inverseOffers; // Active inverse offers for framework.

        // TODO(bmahler): Make this private to enforce that `addExecutor()`
        // and `removeExecutor()` are used, and provide a const view into
        // the executors.
//        hashmap<mesos::SlaveID, hashmap<mesos::ExecutorID, mesos::ExecutorInfo>> executors;

        // NOTE: For the used and offered resources below, we keep the
        // total as well as partitioned by SlaveID.
        // We expose the total resources via the HTTP endpoint, and we
        // keep a running total of the resources because looping over the
        // slaves to sum the resources has led to perf issues (MESOS-1862).
        // We keep the resources partitioned by SlaveID because non-scalar
        // resources can be lost when summing them up across multiple
        // slaves (MESOS-2373).
        //
        // Also note that keeping the totals is safe even though it yields
        // incorrect results for non-scalar resources.
        //   (1) For overlapping set items / ranges across slaves, these
        //       will get added N times but only represented once.
        //   (2) When an initial subtraction occurs (N-1), the resource is
        //       no longer represented. (This is the source of the bug).
        //   (3) When any further subtractions occur (N-(1+M)), the
        //       Resources simply ignores the subtraction since there's
        //       nothing to remove, so this is safe for now.

        // TODO(mpark): Strip the non-scalar resources out of the totals
        // in order to avoid reporting incorrect statistics (MESOS-2623).

//        // Active task / executor resources.
//        Resources totalUsedResources;
//
//        // Note that we maintain multiple copies of each shared resource in
//        // `usedResources` as they are used by multiple tasks.
//        hashmap<mesos::SlaveID, Resources> usedResources;
//
//        // Offered resources.
//        Resources totalOfferedResources;
//        hashmap<mesos::SlaveID, Resources> offeredResources;


    private:
        Framework(Master* const _master,
                  /*const Flags& masterFlags,*/
                  const mesos::FrameworkInfo& _info,
                  State state,
                  const process::Time& time)
                : master(_master),
                  info(_info),
                  /*roles(protobuf::framework::getRoles(_info)),
                  capabilities(_info.capabilities()),*/
                  state(state),
                  registeredTime(time),
                  reregisteredTime(time)
                  /*completedTasks(masterFlags.max_completed_tasks_per_framework),
                  unreachableTasks(masterFlags.max_unreachable_tasks_per_framework)*/
        {
            foreach (const std::string& role, roles) {
                // NOTE: It's possible that we're already being tracked under the role
                // because a framework can unsubscribe from a role while it still has
                // resources allocated to the role.
                if (!isTrackedUnderRole(role)) {
                    trackUnderRole(role);
                }
            }
        }

        Framework(const Framework&);              // No copying.
        Framework& operator=(const Framework&); // No assigning.
    };
}


#endif //CHAMELEON_MASTER_HPP
