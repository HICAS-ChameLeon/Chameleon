/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-1-15
 * Description：super_master
 */

#ifndef CHAMELEON_SUPER_MASTER_HPP
#define CHAMELEON_SUPER_MASTER_HPP
// C++ 11 dependencies
#include <iterator>
#include <vector>
#include <set>
#include <sstream>
#include <unordered_map>
// google
#include <glog/logging.h>
#include <gflags/gflags.h>

// stout dependencies
#include <stout/gtest.hpp>
#include <stout/json.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>
#include <stout/os.hpp>
#include <stout/uuid.hpp>

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
#include <super_master_related.pb.h>
#include <hardware_resource.pb.h>
#include <runtime_resource.pb.h>
#include <messages.pb.h>

// chameleon headers
#include <configuration_glog.hpp>
#include <scheduler.pb.h>


using std::string;
using std::set;
using std::vector;
using std::unordered_map;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;
using process::Subprocess;
using process::subprocess;
using process::http::Request;
using process::http::OK;
using process::http::Response;

//using namespace process::http;

//using process::http::Request;
//using process::http::OK;
using process::http::InternalServerError;


namespace chameleon {

    extern int32_t cluster_levels = 1;

    // each Node is a machine node, maybe Super Master, Master, Slave
    class Node{
    public:
        // node ip
        string m_ip;

        // node port
        int32_t m_port;

        enum {
            SUPERMASTER,
            MASTER,
            SLAVE
        } m_status;

        // when Node is Slave, it has hardware resources
        const HardwareResourcesMessage &m_hardware;

        //when Node is Slave, it has runtime resources
        RuntimeResourcesMessage m_runtime;

        explicit Node(string ip,
                      int32_t port,
                      const HardwareResourcesMessage &hardwareResourcesMessage
        ) : m_ip(ip),
            m_port(port),
            m_hardware(hardwareResourcesMessage) {}

        void set_runtime(const RuntimeResourcesMessage &runtimeResourcesMessage){
            m_runtime = runtimeResourcesMessage;
        }

        bool operator==(const Node& node){
            if(this->m_ip.compare(node.m_ip)==0 && this->m_port==node.m_port){
                return true;
            }
            return false;
        }

        ~Node() {}

    private:

    };

    class SuperMaster :public ProtobufProcess<SuperMaster> {
    public:
        explicit SuperMaster(const string& initiator) : ProcessBase("super_master") ,m_initiator(initiator){
                m_super_master_cwd = os::getcwd();
        }

        // set level from FlAGS_level, level is the required flag of launch super_master
        void set_level(const int32_t& level);

        // set webui_path from FlAGS_webui_path, webui_path is the required flag of launch super_master
        void set_webui_path(const string& path);

        // set master_path from FlAGS_master_path, master_math is the required flag of launch super_master
        void set_master_path(const string& path);

        const string get_web_ui() const;

        const string get_cwd();

        virtual void initialize() override;

        virtual ~SuperMaster() {
            LOG(INFO) << " ~SuperMaster";
        }

        Future<bool> done() {
//            LOG(INFO) << "are we done yet? ";
            return shouldQuit.future();
        }

        void shutdown() {
//            LOG(INFO) << "Shutting down server..." ;
            this->shouldQuit.set(true);
        }

    private:
        Promise<bool> shouldQuit;

        // represent the UUID(random) of the cuttent super_master
        string m_uuid;

        // represent the absolute path for the super_master executable
        string m_super_master_cwd;

        // represent the absolute path for the master executable
        string m_master_path;

        // represent the absolute path for the webui
        string m_webui_path;

        // represent the level want to change from one
        int32_t m_level;

        UPID m_framework;

        // represent the masters administered by the current super_master.
        vector<UPID> m_masters;

        // if the next level of the current master are occupied by masters, then is _next_level_master is true.
        bool is_next_level_master;

        // represent the current number of level
        int32_t m_levels;

        // represent the master starting the super_master
        string m_initiator;

        // represent the slaves administered by the current super_master
        vector<SlaveInfo> m_admin_slaves;

        // represent the current number of masters
        int32_t m_masters_size;

        // represent the current number of super_masters
        int32_t m_super_masters_size;

        // key: master:ip , value: vector<SlavesInfoControlledByMaster>
        unordered_map<string,vector<SlavesInfoControlledByMaster>> m_classification_slaves;

        // represent the masters created by the super_master
        vector<string> m_vector_masters;

        // key: super_master:ip , value: vector<MasterInfoControlledBySuperMaster>
        unordered_map<string,vector<MasterInfoControlledBySuperMaster>> m_classification_masters;

        // represent the super_masters administered by the current super_master
        vector<string> m_vector_super_master;

//        vector<Node> m_master;
        //  key: master:ip:port , value: vector<Node>
        unordered_map<string,vector<Node>> m_master_slave;

        //framework related
        unordered_map<string,string> m_master_framework;

        // represent whether all masters were successfully launch
        bool is_launch_master = true;

        // represent whether all super_masters were successfully launch
        bool is_launch_super_master = true;

        // when change the level from one to two, classify masters
        void classify_masters();

        // when change the level from one to three, classify super_masters
        void classify_super_masters();

        // select the masters to launch and launch them
        void create_masters();

        // when launch new masters successfully or when don't need to launch master, send the slaves' info to it
        void send_super_master_control_message();

        // select the only master when change two level to one level, but now select the self, don't use it
        const string select_master();

        // send message to master to kill it when change two level to one level
        void send_terminating_master(string master_ip);

        // store the information when received hardware resources
        void received_hardware_resources(const UPID &from, const HardwareResourcesMessage &message);

        // store the information when received runtime resources
        void received_runtime_resources(const UPID &from, const RuntimeResourcesMessage &message);

        // framework related
        // super_master send the master address to Framework scheduler driver when received Call message
        void received_call(const UPID &from, const mesos::scheduler::Call &call);

        // classify masters to run different framework
        void classify_masters_framework();

        // determine whether all masters were successfully launch
        void launch_master_results(const UPID &from, const string &message);

        // LOG(INFO) the result of launch master
        void is_launch();

        // when master received the SuperMasterControlMessage, they will send MasterRegisteredMessage to super_master to register
        void registered_master(const UPID &forom, const MasterRegisteredMessage &master_registered_message);

        // determine if the master is registered for the first time
        Future<bool> is_repeated_registered(const UPID &upid);

        // launch the exectuables of maters administered by the current super_master
        void launch_masters();

        // record masters when received MasterRegisteredMessage message
        void record_master(const Future<bool> &future, const UPID &from,
                           const MasterRegisteredMessage &master_registered_message);

        // when change level, terminate masters and launch new master
        void terminating_master(const UPID &from, const OwnedSlavesMessage &message);

    };


}


#endif //CHAMELEON_SUPER_MASTER_HPP
