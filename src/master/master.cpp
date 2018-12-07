/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */

#include "master.hpp"

namespace chameleon {
    void Master::initialize() {
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        install<ParticipantInfo>(&Master::register_participant, &ParticipantInfo::hostname);

        install<HardwareResourcesMessage>(&Master::update_hardware_resources);
        install<JobMessage>(&Master::job_submited);
        install<RuntimeResourcesMessage>(&Master::received_heartbeat);


        // http://172.20.110.228:6060/master/hardware-resources
        route(
                "/hardware-resources",
                "get the topology resources of the whole topology",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    if (!this->m_hardware_resources.empty()) {
                        JSON::Array array;
                        for (auto it = this->m_hardware_resources.begin();
                             it != this->m_hardware_resources.end(); it++) {
                            array.values.push_back(it->second);
                        }
                        result.values["quantity"] = array.values.size();
                        result.values["content"] = array;
                    } else {
                        result.values["quantity"] = 0;
                        result.values["content"] = JSON::Object();
                    }
                    return OK(stringify(result));
                });

        // http://172.20.110.228:6060/master/runtime-resources
        route(
                "/runtime-resources",
                "get the runtime resources of the whole topology",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    if (!this->m_runtime_resources.empty()) {
                        JSON::Array array;
                        for (auto it = this->m_runtime_resources.begin();
                             it != this->m_runtime_resources.end(); it++) {
                            array.values.push_back(it->second);
                        }
                        result.values["quantity"] = array.values.size();
                        result.values["content"] = array;
                    } else {
                        result.values["quantity"] = 0;
                        result.values["content"] = JSON::Object();
                    }
                    return OK(stringify(result));
                });



//     install("stop", &MyProcess::stop);
        install("stop", [=](const UPID &from, const string &body) {
            terminate(self());
        });

    }


    void Master::register_participant(const string &hostname) {
        DLOG(INFO) << "master receive register message from " << hostname;
    }

    void
    Master::update_hardware_resources(const UPID &from, const HardwareResourcesMessage &hardware_resources_message) {
        DLOG(INFO) << "enter update_hardware_resources";

        auto slaveid = hardware_resources_message.slave_id();
        if (m_hardware_resources.find(slaveid) == m_hardware_resources.end()) {
            JSON::Object object = JSON::protobuf(hardware_resources_message);
//                string object_str = stringify(object);
//                DLOG(INFO) << object_str;
            m_hardware_resources.insert({slaveid, object});
            m_alive_slaves.insert(slaveid);
        }
    }

    void Master::job_submited(const UPID &from, const JobMessage &job_message) {
        LOG(INFO) << "got a job from " << from;
        send(*msp_spark_master, job_message);
        LOG(INFO) << "sent the job to the test master 172.20.110.228 successfully!";
        JobMessage slave_job_message;
        slave_job_message.CopyFrom(job_message);
        slave_job_message.set_master_ip("172.20.110.228");
        slave_job_message.set_is_master(false);
        LOG(INFO) << "slave_job_message.is_master = " << slave_job_message.is_master();
        send(*msp_spark_slave, slave_job_message);
        LOG(INFO) << "sent the job to the test slave 172.20.110.79 successfully!";
    }

    void Master::received_heartbeat(const UPID &slave, const RuntimeResourcesMessage &runtime_resouces_message) {
        LOG(INFO) << "received a heartbeat message from " << slave;
        auto slave_id = runtime_resouces_message.slave_id();
        m_runtime_resources[slave_id]= JSON::protobuf(runtime_resouces_message);
        m_proto_runtime_resources[slave_id] = runtime_resouces_message;
    }
}

using namespace chameleon;

int main(int argc, char **argv) {

    chameleon::set_storage_paths_of_glog("master");// provides the program name
    chameleon::set_flags_of_glog();

    os::setenv("LIBPROCESS_PORT", stringify(6060));
    process::initialize("master");

    Master master;
    PID<Master> cur_master = process::spawn(master);
    LOG(INFO) << "Running master on " << process::address().ip << ":" << process::address().port;

    const PID<Master> master_pid = master.self();
    LOG(INFO) << master_pid;
//    LOG(ERROR) << "error test";
    process::wait(master.self());
    return 0;
}