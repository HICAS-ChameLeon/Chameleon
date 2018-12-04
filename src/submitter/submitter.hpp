/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-12-4
 * Description： submitter
 */

#ifndef CHAMELEON_SUBMITTER_HPP
#define CHAMELEON_SUBMITTER_HPP
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>


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
#include <job.pb.h>

// chameleon headers
#include <configuration_glog.hpp>
using std::string;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;
using process::Future;
using process::Promise;
using namespace process::http;

namespace chameleon {
    class Submitter :public ProtobufProcess<Submitter>{
    public:
        explicit Submitter() : ProcessBase("submitter") {}

        virtual ~Submitter(){
        }

        virtual void initialize();

    protected:
        void finalize() override {
            LOG(INFO)<<"submitter finalized ";
        }

    private:
         const string spark_path = "/home/lemaker/software/spark-2.3.0-bin-hadoop2.7.tgz";
        const string DEFAULT_MASTER="master@172.20.110.228:6060";
        shared_ptr<UPID> msp_masterUPID;

    };

}

#endif //CHAMELEON_SUBMITTER_HPP
