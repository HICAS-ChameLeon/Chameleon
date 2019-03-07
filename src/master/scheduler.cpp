/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：WeiGuo Wang 2573777501@qq.com
 * Date       ：19-3-6
 * Description：scheduler
 */
#include "scheduler.hpp"
#include "master.hpp"

using namespace chameleon::scheduler;

namespace chameleon{
    namespace scheduler{
        void Scheduler::initialize() {
            m_min_use_rate = 100.0;
        }

        Slave* Scheduler::find_slave_to_run_framework() {
            double cur_mem_rate;

            foreachvalue(Slave* slave, m_slaves.registered) {

                RuntimeResourcesMessage rrm = m_master->get_runtime_info(slave->m_pid);

                cur_mem_rate = rrm.mem_usage().mem_available() / rrm.mem_usage().mem_total();

                if (m_min_use_rate > cur_mem_rate) {
                    m_min_use_rate = cur_mem_rate;
                    m_slave_pid = slave->m_pid;
                }
                if (m_slave_pid == nullptr) {
                    LOG(INFO) << "There is no suite slave to run this framework";
                    return nullptr;
                }
            }
            return m_slaves.registered.get(m_slave_pid);
        }

//        string Master::find_min_cpu_and_memory_rates() {
//            double min_sum_rate = 100.0;
//            string res = "";
//            for (auto it = m_proto_runtime_resources.begin(); it != m_proto_runtime_resources.end(); it++) {
//                double cur_cpu_rate;
//                double cur_mem_rate;
//                auto cur_message = it->second;
//                cur_cpu_rate = cur_message.cpu_usage().cpu_used() * 0.01;
//                cur_mem_rate = static_cast<double>(cur_message.mem_usage().mem_available()) /
//                               static_cast<double >(cur_message.mem_usage().mem_total());
//                double cur_sum_rate = 50 * cur_cpu_rate + 50 * cur_mem_rate;
//                LOG(INFO) << it->first << " cpu usage is " << cur_cpu_rate << " memory usage is " << cur_mem_rate;
//                LOG(INFO) << it->first << " cur_sum_rate is " << cur_sum_rate;
//                if (min_sum_rate > cur_sum_rate) {
//                    min_sum_rate = cur_sum_rate;
//                    res = it->first;
//                }
//            }
//            if (res.empty()) {
//                LOG(INFO) << " calculate the best machine to schedule the new job failed!";
//                return res;
//            }
//            return res;
//        }
    }
}