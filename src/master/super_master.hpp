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

    class SuperMaster :public ProtobufProcess<SuperMaster> {
    public:
        explicit SuperMaster(const string& initiator) : ProcessBase("super_master") ,m_initiator(initiator){
                m_super_master_cwd = os::getcwd();
        }

        void set_level(const int32_t& level);

        void set_webui_path(const string& path);

        const string get_web_ui() const;

        virtual void initialize() override;

        const string get_cwd();

        void set_master_path(const string& path);
        void set_first_to_second_master(const string& master);
        void registered_master(const UPID &forom, const MasterRegisteredMessage &master_registered_message);

        Future<bool> is_repeated_registered(const UPID &upid);

        void launch_masters();

        void record_master(const Future<bool> &future, const UPID &from,
                           const MasterRegisteredMessage &master_registered_message);

        void terminating_master(const UPID &from, const OwnedSlavesMessage &message);

        struct Node {
            string node_ip;
            int32_t node_port;
            bool is_super_master;
        } nodes;

        virtual ~SuperMaster() {
            LOG(INFO) << " ~SuperMaster";
        }

    private:

        string m_uuid;
        // the absolute path for the super_master executable
        string m_super_master_cwd;
        string m_master_path;

        string m_webui_path;
        int32_t m_level;

        UPID m_framework;
        // represent the masters administered by the current super_master.
        vector<UPID> m_masters;

        // if the next level of the current master are occupied by masters, then is _next_level_master is true.
        bool is_next_level_master;

        // represent the current number of level
        int32_t m_levels;
        string m_initiator;

        vector<SlaveInfo> m_admin_slaves;

        // represent the current number of masters
        int32_t m_masters_size;
        // represent the current number of super_masters
        int32_t m_super_masters_size;

        // key: master:ip , value: vector<SlavesInfoControlledByMaster>
        unordered_map<string,vector<SlavesInfoControlledByMaster>> m_classification_slaves;
        vector<string> m_vector_masters;
        // key: super_master:ip , value: vector<MasterInfoControlledBySuperMaster>
        unordered_map<string,vector<MasterInfoControlledBySuperMaster>> m_classification_masters;
        vector<string> m_vector_super_master;

        vector<Node> m_nodes;
//        vector<Node,vector<Node>> m_nodes;

        //framework related
        unordered_map<string,string> m_master_framework;
        bool is_launch_master = true;
        bool is_launch_super_master = true;

        void classify_masters();
        //change to three levels related
        void classify_super_masters();

        void create_masters();
        void send_super_master_control_message();

        const string select_master();
        void send_terminating_master(string master_ip);

        //framework related
        void received_call(const UPID &from, const mesos::scheduler::Call &call);
        void classify_masters_framework();
        void launch_master_results(const UPID &from, const string &message);
        void is_launch();
    };


}


#endif //CHAMELEON_SUPER_MASTER_HPP
