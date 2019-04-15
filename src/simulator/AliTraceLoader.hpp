//
// Created by Heldon on 19-4-12.
//

#ifndef CHAMELEON_ALITRACELOADER_HPP
#define CHAMELEON_ALITRACELOADER_HPP

#include <string>
#include <map>

#include "AliTracePreProcessor.h"

using std::map;
using std::string;

namespace AliSim{
    class AliTraceLoader {
    private:
        string m_trace_path;
    public:
        AliTraceLoader();
        explicit AliTraceLoader(const string& trace_path);

        void Load_machine(multimap<uint64_t, Machine_mega>* machine_map);
        void Load_task(multimap<uint64_t, Task_identifier>* task_map);
    };
}



#endif //CHAMELEON_ALITRACELOADER_HPP
