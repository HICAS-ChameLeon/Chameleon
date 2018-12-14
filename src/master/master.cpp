/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */

#include "master.hpp"

DEFINE_int32(port, 0, "master port");

/*
 * Function name  : ValidateInt
 * Author         : weiguow
 * Date           : 2018-12-13
 * Description    : Determines whether the input parameter is valid
 * Return         : True or False*/
static bool ValidateInt(const char *flagname, gflags::int32 value) {
    if (value >= 0 && value < 32768) {
        return true;
    }
    printf("Invalid value for --%s: %d\n", flagname, (int) value);
    return false;
}

static const bool port_dummyInt = gflags::RegisterFlagValidator(&FLAGS_port, &ValidateInt);

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
                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
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
                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
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

//    void Master::job_submited(const UPID &from, const JobMessage &job_message) {
//        LOG(INFO) << "got a job from " << from;
//        send(*msp_spark_master, job_message);
//        LOG(INFO) << "sent the job to the test master 172.20.110.228 successfully!";
//        JobMessage slave_job_message;
//        slave_job_message.CopyFrom(job_message);
//        slave_job_message.set_master_ip("172.20.110.228");
//        slave_job_message.set_is_master(false);
//        LOG(INFO) << "slave_job_message.is_master = " << slave_job_message.is_master();
//        send(*msp_spark_slave, slave_job_message);
//        LOG(INFO) << "sent the job to the test slave 172.20.110.79 successfully!";
//    }

    void Master::job_submited(const UPID &from, const JobMessage &job_message) {
        LOG(INFO) << "got a job from " << from;

        JobMessage slave_job_message;
        slave_job_message.CopyFrom(job_message);
        slave_job_message.set_master_ip("172.20.110.228");
        slave_job_message.set_is_master(false);

        // find the best machine whose sum usage of cpu and memory is the lowest.
        Try<string> best_machine = find_min_cpu_and_memory_rates();
        if (best_machine.isError()) {
            LOG(FATAL) << " cannot find a appropriate machine to run the job!";
            return;
        }
        string str_spark_slave = "slave@";
        str_spark_slave.append(best_machine.get());
        str_spark_slave.append(":6061");
        UPID spark_slave(str_spark_slave);
        LOG(INFO) << "slave_job_message.is_master = " << slave_job_message.is_master();
        send(spark_slave, slave_job_message);
        LOG(INFO) << "sent the job to the test slave " << str_spark_slave << " successfully!";
    }

    void Master::received_heartbeat(const UPID &slave, const RuntimeResourcesMessage &runtime_resouces_message) {
        LOG(INFO) << "received a heartbeat message from " << slave;
        auto slave_id = runtime_resouces_message.slave_id();
        m_runtime_resources[slave_id] = JSON::protobuf(runtime_resouces_message);
        m_proto_runtime_resources[slave_id] = runtime_resouces_message;
    }

    Try<string> Master::find_min_cpu_and_memory_rates() {
        double min_sum_rate = 100.0;
        string res = "";
        for (auto it = m_proto_runtime_resources.begin(); it != m_proto_runtime_resources.end(); it++) {
            double cur_cpu_rate;
            double cur_mem_rate;
            auto cur_message = it->second;
            cur_cpu_rate = cur_message.cpu_usage().cpu_used() * 0.01;
            cur_mem_rate = static_cast<double>(cur_message.mem_usage().mem_available()) /
                           static_cast<double >(cur_message.mem_usage().mem_total());
            double cur_sum_rate = 50 * cur_cpu_rate + 50 * cur_mem_rate;
            LOG(INFO) << it->first << " cpu usage is " << cur_cpu_rate << " memory usage is " << cur_mem_rate;
            LOG(INFO) << it->first << " cur_sum_rate is " << cur_sum_rate;
            if (min_sum_rate > cur_sum_rate) {
                min_sum_rate = cur_sum_rate;
                res = it->first;
            }
        }
        if (res.empty()) {
            LOG(ERROR) << " calculate the best machine to schedule the new job failed!";
            return Error("The whole cluster has no machine");
        }
        return res;
    }
}

using namespace chameleon;


int main(int argc, char **argv) {
    chameleon::set_storage_paths_of_glog("master");// provides the program name
    chameleon::set_flags_of_glog();

    google::SetUsageMessage("usage : Option[name] \n"
                            "--port     the port used by the program");
    google::SetVersionString("Chameleon v1.0");
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::CommandLineFlagInfo info;

    if (GetCommandLineFlagInfo("port", &info) && info.is_default) {
        LOG(INFO) << "To run this program , must set parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    } else {
        if (GetCommandLineFlagInfo("port", &info) && !info.is_default) {
            os::setenv("LIBPROCESS_PORT",stringify(FLAGS_port));
            process::initialize("master");

            Master master;
            PID<Master> cur_master = process::spawn(master);
            LOG(INFO) << "Running master on " << process::address().ip << ":" << process::address().port;

            const PID<Master> master_pid = master.self();
            LOG(INFO) << master_pid;
            process::wait(master.self());
        } else {
            LOG(INFO) << "To run this program , must set all parameters correctly "
                         "\n read the notice " << google::ProgramUsage();
        }
    }
    return 0;
}

