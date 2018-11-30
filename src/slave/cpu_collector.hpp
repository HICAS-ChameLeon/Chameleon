//
// Created by weiguow on 18-11-19.
//

#ifndef LIBPROCESS_START_CPU_INFO_H
#define LIBPROCESS_START_CPU_INFO_H

#include <fstream>
#include <sstream>
#include <iostream>

#include <error.h>
#include <list>
#include <string>
#include <vector>

#include <sstream>

#include <stout/option.hpp>
#include <stout/try.hpp>
#include <stout/numify.hpp>
#include <stout/os/int_fd.hpp>

#include <process/io.hpp>
#include <process/future.hpp>
#include <process/subprocess.hpp>

#include <hardware_resource.pb.h>


using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::list;
using std::istringstream;
using std::ostream;

using strings::tokenize;

using process::subprocess;
using process::Subprocess;


namespace chameleon {
    class cpu_collector {

    public:
        vector<CPUInfo> get_cpu_info() {
            vector<CPUInfo> result_ci;

            CPUInfo ci;
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
            Option<string> L1dcache;
            Option<string> L1icache;
            Option<string> L2cache;
            Option<string> L3cache;

            std::string line;
            while (std::getline(file, line)) {

                std::vector<std::string> split = strings::split(line, ":");
                if (line.find("processor") == 0) {
                    cpuID = split.back().data();
                } else if (line.find("physical id") == 0) {
                    physicalID = split.back().data();
                } else if (line.find("core id") == 0) {
                    coreID = split.back().data();
                } else if (line.find("cpu cores") == 0) {
                    cpucores = split.back().data();
                } else if (line.find("model name") == 0) {
                    modelname = split.back().data();
                } else if (line.find("cpu MHz") == 0) {
                    cpuMHz = split.back().data();
                }

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
                        L1dcache = strings::trim(vec.back());
                    } else if (vec.front() == "L1i cache") {
                        L1icache = strings::trim(vec.back());
                    } else if (vec.front() == "L2 cache") {
                        L2cache = strings::trim(vec.back());
                    } else if (vec.front() == "L3 cache") {
                        L3cache = strings::trim(vec.back());
                    }
                }

                //finally create a CPU if we have all the information.
                if (cpuID.isSome() && coreID.isSome() && physicalID.isSome() && cpucores.isSome() &&
                    modelname.isSome() && cpuMHz.isSome() && L1dcache.isSome() && L2cache.isSome() &&
                    L1dcache.isSome() && L3cache.isSome()) {


                    ci.set_cpuid(cpuID.get());
                    ci.set_coreid(coreID.get());
                    ci.set_physicalid(physicalID.get());
                    ci.set_cpucores(cpucores.get());
                    ci.set_modelname(modelname.get());
                    ci.set_cpumhz(cpuMHz.get());
                    ci.set_l1dcache(L1dcache.get());
                    ci.set_l1icache(L1icache.get());
                    ci.set_l2cache(L2cache.get());
                    ci.set_l3cache(L3cache.get());

                    result_ci.push_back(ci);

                    cpuID = None();
                    coreID = None();
                    physicalID = None();
                    cpucores = None();
                    modelname = None();
                    cpuMHz = None();
                    L1dcache = None();
                    L1icache = None();
                    L2cache = None();
                    L3cache = None();
                }
            }
            return result_ci;
        }
    };
}

#endif //LIBPROCESS_START_CPU_INFO_H

