//
// Created by Heldon on 19-4-12.
//

#include <cstdio>

#include <stout/strings.hpp>
#include <boost/lexical_cast.hpp>

#include <glog/logging.h>

#include "AliTraceLoader.hpp"

using boost::lexical_cast;

namespace AliSim{

    AliTraceLoader::AliTraceLoader() {}

    AliTraceLoader::AliTraceLoader(const string& trace_path) : m_trace_path(trace_path){

    }

    void AliTraceLoader::Load_machine(multimap<uint64_t, Machine_mega> *machine_map) {

        string file_path = m_trace_path + "test.csv";
        FILE* machine_mega_file;
        if((machine_mega_file = fopen(file_path.c_str(), "r")) == nullptr)
            LOG(FATAL) << "Failed to open test.csv";
        else
            LOG(INFO) << "Success to open : " << file_path;

        int64_t num_line = 1;
        char line[150];
        vector<string> line_cols;  //字段
        int32_t machine_count = 0;  //机器数量

        while (!feof(machine_mega_file)){

            if (fscanf(machine_mega_file, "%[^\n]%*[\n]", &line[0]) > 0){
                line_cols = strings::tokenize(line, ",");  //以逗号分割每行
                if(line_cols.size() != 7)
                    //不是7个字段则说明出错
                    LOG(FATAL) << "Unexpected structure of sever event on line" << num_line << ": found"
                               << line_cols.size() << " columns";
                else{
                    Machine_mega machine_mega;
                    try {
                        machine_mega.machine_id = lexical_cast <string> (line_cols[0]);
                        machine_mega.time_stamp = lexical_cast <uint64_t>(line_cols[1]);
                        machine_mega.cpu_num = lexical_cast <int32_t>(line_cols[4]);
                        machine_mega.mem_size = lexical_cast <int32_t>(line_cols[5]);
                        if(machine_mega.status == "USING")
                            machine_mega.status = Machine_status::USING;

                        //将machine插入machine_map中
                        pair<uint64_t, Machine_mega> temp_machine_mega(machine_mega.time_stamp, machine_mega);
//                      machine_map->insert(pair<uint64_t, Machine_mega>(machine_mega.time_stamp, machine_mega));
                        machine_map->insert(temp_machine_mega);
                        if(machine_mega.time_stamp == 0)
                            machine_count++;

                    }
                    catch(bad_cast& e) {
                        LOG(INFO)<< e.what();
                        LOG(INFO)<< "num line : " << num_line;
                    }
                }

            }
            num_line++;
        }
        LOG(INFO)<<"Start up! The cluster totally has "<< machine_count << " machines\n";
        fclose(machine_mega_file);
    }

}