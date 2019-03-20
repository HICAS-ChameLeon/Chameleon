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
#include <unordered_map>
#include <unordered_set>

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
#include <stout/path.hpp>

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
#include <hardware_resource.pb.h>
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
#include <chameleon_resources.hpp>
#include <slave_object.hpp>
#include <scheduler_interface.hpp>
#include <coarse_grained_scheduler.hpp>

using std::string;
using std::set;
using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::shared_ptr;
using std::make_shared;
using std::list;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;
//using namespace process::http;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;
using process::Subprocess;

namespace chameleon {

    class Framework;
    class Master;

//    class Slave {
//    public:
//        Slave(Master *const _master,
//              const mesos::SlaveInfo &_info,
//              const process::UPID &_pid) :
//                master(_master),
//                info(_info),
//                id(_info.id()),
//                pid(_pid) {
//        };
//
//        ~Slave();
//
//        Master *const master;
//        const mesos::SlaveID id;
//        const mesos::SlaveInfo info;
//        process::UPID pid;
//
//    private:
//
//        //Constructor cannot be redeclared
////        Slave(Master *const _master,
////              const mesos::SlaveInfo &_info)
////                : master(_master),
////                  info(_info),
////                  id(info.id()){}
//
//        Slave(const Slave&);
//
//        Slave &operator=(const Slave&);
//
//    };


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

            m_master_cwd = os::getcwd();
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
         * get a heartbeat message from a slave. The heartbeat message contains the runtime resource usage statistics of the slave.
         * @param slave
         * @param runtime_resouces_message represents the runtime resource usage statistics for the slave
         */
        void received_heartbeat(const UPID &slave, const RuntimeResourcesMessage &runtime_resouces_message);

        /**
         * make random ID-weiguow-2019/2/24
         * */
        mesos::FrameworkID newFrameworkId();

        Framework *getFramework(const mesos::FrameworkID &frameworkId);

        hashmap<string, mesos::Offer*> offers;

        /**
         * save Frameworkinfo-weiguow-2019-2-22
         * */
        struct Frameworks {
            // key: framework ID, value: Framework
            hashmap<string, Framework*> registered;

//            BoundedHashMap<string, Framework*> completed;

        } frameworks;

        void Offer(const mesos::FrameworkID &frameworkId);

        void receive(
                const process::UPID &from,
                const mesos::scheduler::Call &call);

        void subscribe(
                const process::UPID &from,
                const mesos::scheduler::Call::Subscribe &subscribe);

        void teardown(Framework* framework);

        void accept(Framework *framework, mesos::scheduler::Call::Accept accept);

        void decline(Framework* framework,const mesos::scheduler::Call::Decline& decline);

        void shutdown(Framework* framework,const mesos::scheduler::Call::Shutdown& shutdown);

        void statusUpdate(mesos::internal::StatusUpdate update, const UPID &pid);

        void statusUpdateAcknowledgement(
                const UPID &from,
                const mesos::SlaveID &slaveId,
                const mesos::FrameworkID &frameworkId,
                const mesos::TaskID &taskId,
                const string &uuid);

        void acknowledge(Framework *framework, const mesos::scheduler::Call::Acknowledge &acknowledge);

        void add_framework(Framework *framework);

        void remove_framework(Framework *framework);

        void deactivate(Framework* framework, bool rescind);

        void get_select_master(const UPID& from, const string& message);
        void get_slave_infos(const UPID& from, const string& message);
        // super_master related
        void set_super_master_path(const string& path);

        const string get_cwd() const;

        void set_webui_path(const string& path);

        const string get_web_ui() const;

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

        // key: slave_ip, value: hardware_resources
        unordered_map<string, JSON::Object> m_hardware_resources;
        unordered_map<string, HardwareResourcesMessage> m_proto_hardware_resources;
        set<string> m_alive_slaves;
        // key: slave_uuid, value: shared_ptr<SlaveObject>
        unordered_map<string, shared_ptr<SlaveObject>> m_slave_objects;
        // key: framework ID value:  the unordered_set of slave_uuids. It records the slaves where the framework is running on.
        unordered_map<string,unordered_set<string>> m_framework_to_slaves;

        // key: slave_ip, value: runtime_resources
        unordered_map<string, JSON::Object> m_runtime_resources;
        unordered_map<string, RuntimeResourcesMessage> m_proto_runtime_resources;

        // scheduler related
        shared_ptr<SchedulerInterface> m_scheduler;

        int64_t nextFrameworkId;

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

        // super_master related
        string m_super_master_path;
        void super_master_control(const UPID &super_master, const SuperMasterControlMessage &super_master_control_message);

        void received_registered_message_from_super_master(const UPID &super_master, const AcceptRegisteredMessage &message);
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
                    ACTIVE
        };

        Framework(Master *const master,
                  const mesos::FrameworkInfo &info,
                  const process::UPID &_pid,
                  const process::Time& time = process::Clock::now()
        ) : Framework(master, info, ACTIVE, time) {
            pid = _pid;
        }

        ~Framework() {}

        template<typename Message>
        void send(const Message &message) {
            if (!connected()) {
                LOG(WARNING) << "Master attempted to send message to disconnected"
                             << " framework ";
            } else {
                master->send(pid.get(), message);
                LOG(INFO) << "master send message to " << pid.get();
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

        process::Time registeredTime;
        process::Time unregisteredTime;

    private:
        Framework(Master *const _master,
                  const mesos::FrameworkInfo &_info,
                  State state,
                  const process::Time& time
        ) : master(_master),
            info(_info),
            state(state),
            registeredTime(time){}

        Framework(const Framework &);

        Framework &operator=(const Framework &);

    };

//    inline std::ostream &operator<<(std::ostream &stream, const Slave &slave) {
//        return stream << slave.id.value() << " at " << slave.pid
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
