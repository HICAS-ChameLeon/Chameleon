/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguow 2573777501@qq.com
 * Date       ：19-3-16
 * Description：one scheduling algorithms are provided
 *              (1) Based on the CPU usage
 */
#include <scheduler.hpp>

namespace chameleon {
    Try<string> CpuScheduler::scheduler(unordered_map <string, RuntimeResourcesMessage> runtime_resource) {
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
}

