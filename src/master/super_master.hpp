/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-1-15
 * Description：super_master
 */

#ifndef CHAMELEON_SUPER_MASTER_HPP
#define CHAMELEON_SUPER_MASTER_HPP
// C++ 11 dependencies
#include <vector>
#include <set>
#include<iterator>
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

// chameleon headers
#include <configuration_glog.hpp>

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


namespace chameleon {

    extern int32_t cluster_levels = 1;

    class SuperMaster :public ProtobufProcess<SuperMaster>{
    public:
        explicit SuperMaster():ProcessBase("super_master"){

        }

        virtual void initialize() override;

        void registered_master(const UPID &forom, const MasterRegisteredMessage &master_registered_message);

        Future<bool> is_repeated_registered(const UPID& upid);

        bool launch_masters();

        void record_master(const Future<bool>& future,const UPID &from, const MasterRegisteredMessage &master_registered_message);

        void terminating_master(const UPID& from,const OwnedSlavesMessage& message);
        virtual ~SuperMaster(){
            LOG(INFO)<<" ~SuperMaster";
        }




    private:

        string m_uuid;
        // represent the super masters or masters administered by the current node.
        vector<UPID> m_masters;

        // if the next level of the current master are occupied by masters, then is _next_level_master is true.
        bool is_next_level_master;

        // represent the current number of level
        int32_t m_levels;
        string m_first_to_second_master;

        vector<SlaveInfo> m_admin_slaves;

        // represent the current number of masters
        int32_t m_masters_size;

        // key: master:ip , value: vector<SlavesInfoControlledByMaster>
        unordered_map<string,vector<SlavesInfoControlledByMaster>> m_classification_slaves;
        vector<string> m_classification_masters;
        void classify_masters();

        void create_masters();
        void send_super_master_control_message();




    };

}


#endif //CHAMELEON_SUPER_MASTER_HPP
