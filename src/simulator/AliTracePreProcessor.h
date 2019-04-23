//
// Created by Heldon on 19-4-12.
//

#ifndef CHAMELEON_ALITRACEPREPROCESSOR_HPP
#define CHAMELEON_ALITRACEPREPROCESSOR_HPP

#include <iostream>
#include <string>
#include <cstdint>

#include <map>
#include <unordered_map>
#include <vector>

using namespace std;

namespace AliSim{
    struct Machine_mega{
        string machine_id;
        uint64_t time_stamp;
        int32_t cpu_num;
        int32_t mem_size;
        string status;

        friend ostream &operator<< (ostream &out, Machine_mega machine_mega){
            cout << "machine_id : " <<machine_mega.machine_id << " ; ";
            cout << "time_stamp : " <<machine_mega.time_stamp << " ; ";
            cout << "cpu_num : " <<machine_mega.cpu_num << " ; ";
            cout << "mem_size : " << machine_mega.mem_size << " ; ";
            cout << "status : " << machine_mega.status << endl;
            return out;
        };
    };

    enum Machine_status{
        USING
    };

    struct Task_identifier{
        string task_name;
        int32_t inst_num;  //任务含有的实例的数量
        string job_name;
        string task_type;
        string status;
        uint64_t start_time;
        uint64_t end_time;
        double plan_cpu;
        double plan_mem;

        friend ostream &operator<< (ostream &out, Task_identifier task_identifier){
            cout << "task name : " <<task_identifier.task_name << " ; ";
            cout << "inst_num : " <<task_identifier.inst_num << " ; ";
            cout << "job_name : " <<task_identifier.job_name << " ; ";
            cout << "status : " << task_identifier.status << " ; ";
            cout << "start_time : " << task_identifier.start_time << endl;
            cout << "end_time : " << task_identifier.end_time << endl;
            cout << "plan_cpu : " << task_identifier.plan_cpu << endl;
            cout << "plan_mem : " << task_identifier.plan_mem << endl;
            return out;
        };
    };

    enum Task_status{
        Terminated,
        Waiting,
        Failed,
        Running
    };
}

#endif //CHAMELEON_ALITRACEPREPROCESSOR_HPP
