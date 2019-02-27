/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-3
 * Description：Test class cpu_collector
 */
// GTEST DEPENDENCIES
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// STOUT DEPENDENCIES
#include <stout/gtest.hpp>


 #include <resource_collector/cpu_collector.hpp>

using std::set;
using std::string;
using std::list;


TEST(CpuCollectorTest,TestClass) {
    chameleon::X86CpuCollector cpus;
    CPUCollection *cpuCollection = cpus.get_cpu_info();
    for (auto i = cpuCollection->cpu_infos().begin(); i != cpuCollection->cpu_infos().end(); i++) {
        Option<int> cpuid = i->cpuid();
        EXPECT_SOME(cpuid);

        Option<int> coreid = i->coreid();
        EXPECT_SOME(coreid);

        Option<string> modelname = i->modelname();
        EXPECT_SOME(modelname);

        Option<int> physicalid = i->physicalid();
        EXPECT_SOME(physicalid);

        Option<int> cpucores = i->cpucores();
        EXPECT_SOME(cpucores);

        Option<double> cpuMHz = i->cpumhz();
        EXPECT_SOME(cpuMHz);

        Option<double> cpuminMHz = i->cpuminmhz();
        EXPECT_SOME(cpuminMHz);

        Option<double> cpumaxMHz = i->cpumaxmhz();
        EXPECT_SOME(cpumaxMHz);

        Option<string> l1dcache = i->l1dcache();
        EXPECT_SOME(l1dcache);

        Option<string> l1icache = i->l1icache();
        EXPECT_SOME(l1icache);

        Option<string> l2cache = i->l2cache();
        EXPECT_SOME(l2cache);

        Option<string> l3cache = i->l3cache();
        EXPECT_SOME(l3cache);
    }
}

TEST(ARMCpuCollectorTest,TestClass) {
    chameleon::ARMCpuCollector cpus;
    CPUCollection *cpuCollection = cpus.get_cpu_info();
    for (auto i = cpuCollection->cpu_infos().begin(); i != cpuCollection->cpu_infos().end(); i++) {
        Option<int> cpuid = i->cpuid();
        EXPECT_SOME(cpuid);

        Option<string> modelname = i->modelname();
        EXPECT_SOME(modelname);

        Option<int> cpucores = i->cpucores();
        EXPECT_SOME(cpucores);

        Option<double> cpuMHz = i->cpumhz();
        EXPECT_SOME(cpuMHz);

        Option<double> cpuminMHz = i->cpuminmhz();
        EXPECT_SOME(cpuminMHz);

        Option<double> cpumaxMHz = i->cpumaxmhz();
        EXPECT_SOME(cpumaxMHz);


        Option<string> l1dcache = i->l1dcache();
        EXPECT_SOME(l1dcache);

        Option<string> l1icache = i->l1icache();
        EXPECT_SOME(l1icache);

        Option<string> l2cache = i->l2cache();
        EXPECT_SOME(l2cache);

    }
}

int main(int argc,char** argv){
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}
