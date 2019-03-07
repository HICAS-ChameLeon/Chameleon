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
    }
}