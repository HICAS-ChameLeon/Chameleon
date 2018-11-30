/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-11-30
 * Description：Get Cpu information from computer
 */
#include <cpu_collector.hpp>

using std::set;
using std::string;
using std::list;

int main() {
    chameleon::Cpu_Collector cpus;
    CPUCollection cpuCollection = cpus.get_cpu_info();
    for(auto i = cpuCollection.cpu_infos().begin();i!=cpuCollection.cpu_infos().end();i++){
        cout << "cpuID:" << i->cpuid() << " coreID:" << i->coreid() << i->physicalid() <<
             " cpucores:" << i->cpucores() <<
             " modelname:" << i->modelname() <<
             " cpuMHz:" << i->cpumhz() <<
             " l1dcache:" << i->l1dcache() <<
             " l1icache:"<< i->l1icache() <<
             " l2cache:" << i->l2cache() <<
             " l3cache:" << i->l3cache() << endl;
    }
}
