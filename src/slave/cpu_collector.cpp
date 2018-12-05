/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-3
 * Description：Get Cpu information from computer
 */
#include <cpu_collector.hpp>

namespace  chameleon {
    CPUCollection* CpuCollector::get_cpu_info()  {
        CPUInfo ci;
        short int cpuquantities = 0;
        CPUCollection* cpuCollection = new CPUCollection();

        std::ifstream file("/proc/cpuinfo");
        if (!file.is_open()) {
            cout << "Failed to open /proc/cpuinfo" << endl;
        }

        Option<string> cpuID;
        Option<string> coreID;
        Option<string> physicalID;
        Option<string> cpucores;
        Option<string> modelname;
        Option<string> cpuMHz;
        Option<string> l1dcache;
        Option<string> l1icache;
        Option<string> l2cache;
        Option<string> l3cache;

        Try<Subprocess> cache = subprocess(
        "lscpu",
        Subprocess::FD(STDIN_FILENO),
        Subprocess::PIPE(),
        Subprocess::FD(STDIN_FILENO)
    );
    process::Future<string> ganied = process::io::read(cache.get().out().get());
    vector<string> lscpu = strings::split(ganied.get(), "\n");
    for (int i = 0; i < lscpu.size(); i++) {
        vector<string> vec = strings::split(lscpu[i], ":");
        if (vec.front() == "L1d cache") {
            l1dcache = strings::trim(vec.back());
            continue;
        } else if (vec.front() == "L1i cache") {
            l1icache = strings::trim(vec.back());\
            continue;
        } else if (vec.front() == "L2 cache") {
            l2cache = strings::trim(vec.back());
            continue;
        } else if (vec.front() == "L3 cache") {
            l3cache = strings::trim(vec.back());
            continue;
        }
    }

    std::string line;

    while (std::getline(file, line)) {

        std::vector<std::string> split = strings::split(line, ":");
        if (line.find("processor") == 0) {
            cpuID = split.back().data();
            continue;
        } else if (line.find("physical id") == 0) {
            physicalID = split.back().data();
            continue;
        } else if (line.find("core id") == 0) {
            coreID = split.back().data();
            continue;
        } else if (line.find("cpu cores") == 0) {
            cpucores = split.back().data();
            continue;
        } else if (line.find("model name") == 0) {
            modelname = split.back().data();
            continue;
        } else if (line.find("cpu MHz") == 0) {
            cpuMHz = split.back().data();
            continue;
        }

        /**
         * finally create a CPU if we have all the information.
         * */
        if (cpuID.isSome() && coreID.isSome() && physicalID.isSome() && cpucores.isSome() &&
            modelname.isSome() && cpuMHz.isSome()) {

            ci.set_cpuid(cpuID.get());
            ci.set_coreid(coreID.get());
            ci.set_physicalid(physicalID.get());
            ci.set_cpucores(cpucores.get());
            ci.set_modelname(modelname.get());
            ci.set_cpumhz(cpuMHz.get());
            ci.set_l1dcache(l1dcache.get());
            ci.set_l1icache(l1icache.get());
            ci.set_l2cache(l2cache.get());
            ci.set_l3cache(l3cache.get());

            cpuCollection->add_cpu_infos()->MergeFrom(ci);

            cpuID = None();
            coreID = None();
            physicalID = None();
            cpucores = None();
            modelname = None();
            cpuMHz = None();

            cpuquantities++;
        }
    }
    cpuCollection->set_cpu_quantity(cpuquantities);
    return  cpuCollection;
    };
}

