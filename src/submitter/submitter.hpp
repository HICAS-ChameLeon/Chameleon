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
#include <stout/os.hpp>
#include <stout/uuid.hpp>
#include <stout/flags/flag.hpp>

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
#include "../common/chameleon_file.hpp"

using std::string;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;

using os::Process;
using os::ProcessTree;

using process::UPID;
using process::PID;

using flags::Warnings;

namespace chameleon{
    class Submitter :public ProtobufProcess<Submitter>{
    public:
        explicit Submitter() : ProcessBase("submitter") {}

        virtual ~Submitter(){
        }

        virtual void initialize();

        void setM_spark_path(const string &m_spark_path) {
            Submitter::m_spark_path = m_spark_path;
        }

        void setDEFAULT_MASTER(const string &DEFAULT_MASTER) {
            Submitter::DEFAULT_MASTER = DEFAULT_MASTER;
        }

    protected:
        void finalize() override {
            LOG(INFO)<<"submitter finalized ";
        }

    private:
        string m_spark_path;
        string DEFAULT_MASTER;
        shared_ptr<UPID> msp_masterUPID;
        id::UUID m_uuid = id::UUID::random();
    };

}

#endif //CHAMELEON_SUBMITTER_HPP
