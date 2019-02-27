/**
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：weiguo
 * Date       ：18-12-5
 * Description：Get ARM Cpu information from computer
 */

#include <resource_collector/cpu_collector.hpp>

namespace chameleon{
    CPUCollection* ARMCpuCollector::get_cpu_info() {
        CPUInfo ci;
        short int cpuquantities = 0;

        CPUCollection* cpuCollection = new CPUCollection();

        std::ifstream file("/proc/cpuinfo");

        if(!file.is_open()){
            LOG(INFO) << "FAILED TO OPEN /proc/cpuinfo";
        }

        Option<int> cpuID;
        Option<int> cpucores;

        Option<string> modelname;

        Option<double> cpumaxMhz;
        Option<double> cpuminMHz;

        Option<string> l1dcache;
        Option<string> l1icache;
        Option<string> l2cache;

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
            if (vec.front() == "CPU max MHz"){
                cpumaxMhz = stod(strings::trim(vec.back()));
                continue;
            } else if (vec.front() == "CPU min MHz"){
                cpuminMHz = stod(strings::trim(vec.back()));
                continue;
            } else if (vec.front() == "CPU(s)") {
                cpucores = stoi(vec.back());
                continue;
            }
        }

        std::string line;

        while (std::getline(file, line)) {

            std::vector<std::string> split = strings::split(line, ":");
            if (line.find("processor") == 0) {
                cpuID = stoi(split.back().data());
                continue;
            } else if (line.find("model name") == 0) {
                modelname = split.back().data();
                continue;
            }

            /**
             * Finally Create a CPU If We Have All The Information.
             * */
            if (cpuID.isSome() && modelname.isSome() && cpuminMHz.isSome() && cpumaxMhz.isSome()) {
                ci.set_cpuid(cpuID.get());
                ci.set_modelname(modelname.get());

                ci.set_cpumaxmhz(cpumaxMhz.get());
                ci.set_cpuminmhz(cpuminMHz.get());

                ci.set_l1dcache("32 KB");
                ci.set_l1icache("32 KB");
                ci.set_l2cache("128 KB–8 MB");

                cpuCollection->add_cpu_infos()->MergeFrom(ci);

                cpuID = None();
                modelname = None();

                cpuquantities++;
            }
        }
        cpuCollection->set_cpu_quantity(cpuquantities);
        return  cpuCollection;
    }
}
