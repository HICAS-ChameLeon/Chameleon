/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguow 2573777501@qq.com
 * Date       ：19-3-16
 * Description：Three scheduling algorithms are provided
 *              (1) Based on the CPU usage
 *              (2) Based on the Memory remaining size
 *              (3) Based on mixed judgment of multiple resources
 */

#include "scheduler.hpp"

namespace chameleon {
    void Scheduler::initialize() {
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;
    }

    void Scheduler::choose_scheduler(const string scheduler_type,
            unordered_map<string, HardwareResourcesMessage> hardware_resources,
            unordered_map<string, RuntimeResourcesMessage> runtime_resource) {

        if (scheduler_type == "BASED_ON_CPU") {
            choose_slave_base_cpu(runtime_resource);

        } else if (scheduler_type == "BASED_ON_MEMROY") {
            choose_slave_base_memory(runtime_resource);

        } else if (scheduler_type == "BASED_ON_METRIC") {
            choose_slave_base_mixed_judgment(hardware_resources, runtime_resource);
        }
    }

    Try<string> Scheduler::choose_slave_base_memory(unordered_map<string, RuntimeResourcesMessage> runtime_resources) {
        double min_use_rate = 100.0;
        string res = "";

        for (auto it = runtime_resources.begin(); it != runtime_resources.end(); it++) {

            double cur_mem_rate;

            auto cur_message = it->second;

            cur_mem_rate = static_cast<double>(cur_message.mem_usage().mem_available()) /
                           static_cast<double >(cur_message.mem_usage().mem_total());

            LOG(INFO) << it->first << " memory usage is " << cur_mem_rate;

            if (min_use_rate > cur_mem_rate) {
                min_use_rate = cur_mem_rate;
                res = it->first;
            }
        }
        if (res.empty()) {
            LOG(ERROR) << " calculate the best machine to schedule the new job failed!";
        }
        return res;
    }

    Try<string> Scheduler::choose_slave_base_cpu(unordered_map<string, RuntimeResourcesMessage> runtime_resources) {
        double min_use_rate = 100.0;
        string res = "";

        for (auto it = runtime_resources.begin(); it != runtime_resources.end(); it++) {
            double cur_cpu_rate;

            auto cur_message = it->second;
            cur_cpu_rate = cur_message.cpu_usage().cpu_used() * 0.01;

            LOG(INFO) << it->first << " cpu usage is " << cur_cpu_rate ;

            if (min_use_rate > cur_cpu_rate) {
                min_use_rate = cur_cpu_rate;
                res = it->first;
            }
        }
        if (res.empty()) {
            LOG(ERROR) << " calculate the best machine to schedule the new job failed!";
        }
        return res;
    }

    Try<string> Scheduler::choose_slave__base_mixed_judgment(
            unordered_map<string, HardwareResourcesMessage> hardware_resources,
            unordered_map<string, RuntimeResourcesMessage> runtime_resources) {

    }
}
