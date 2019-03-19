/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguow 2573777501@qq.com
 * Date       ：19-3-16
 * Description：one scheduling algorithms are provided
 *              (1) Based on the Memory remaining size
 */
#include <scheduler.hpp>

namespace chameleon {
    Try<string> MemoryScheduler::scheduler(unordered_map <string, RuntimeResourcesMessage> runtime_resource) {
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
}

