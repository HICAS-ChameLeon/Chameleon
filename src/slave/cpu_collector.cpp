//
// Created by weiguow on 18-11-26.
//

#include <condition_variable>

#include <gmock/gmock.h>

#include <cpu_collector.hpp>

using std::set;
using std::string;
using std::list;

int main() {
    chameleon::cpu_collector *cpus;

    vector<CPUInfo> cpu11 = cpus->get_cpu_info();

    for (auto i = cpu11.begin(); i != cpu11.end(); i++) {
        cout << "cpuID:" << i->cpuid() << " coreID:" << i->coreid() << i->physicalid() <<
             " cpucores:" << i->cpucores() <<
             " modelname:" << i->modelname() <<
             " cpuMHz: " << i->cpumhz() <<
             " l1dcache: " << i->l1dcache() <<
             " l1icache: "<< i->l1icache() <<
             " l2cache: " << i->l2cache() <<
             " l3cache: " << i->l3cache() << endl;
    }


}
