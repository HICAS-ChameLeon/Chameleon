/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-12-03
 * Description：the general configuration for glog we used for this project.
 */

#ifndef CHAMELEON_CONFIGURATION_GLOG_HPP
#define CHAMELEON_CONFIGURATION_GLOG_HPP

#include <cstring>
#include <glog/logging.h>

namespace chameleon{

    inline void set_storage_paths_of_glog(char* program_name,char* basic = "../log/"){
        google::InitGoogleLogging(program_name);

        char log_dir_path[50];
        strcpy(log_dir_path,basic);
        strcat(log_dir_path,program_name);
        char prefix[70];
        strcpy(prefix,log_dir_path);
        strcat(prefix,"_log_fatal_");
        google::SetLogDestination(google::GLOG_FATAL, prefix);
        strcpy(prefix,log_dir_path);
        strcat(prefix,"_log_error_");
        google::SetLogDestination(google::GLOG_ERROR, prefix);
        strcpy(prefix,log_dir_path);
        strcat(prefix,"_log_warning_");
        google::SetLogDestination(google::GLOG_WARNING, prefix);
        strcpy(prefix,log_dir_path);
        strcat(prefix,"_log_info_");
        google::SetLogDestination(google::GLOG_INFO,prefix);

    }

    inline void set_flags_of_glog(){
        FLAGS_alsologtostderr = true;  // glog print to glog files as well as standard input,output, error output
        FLAGS_colorlogtostderr = true;  // configure the color support
        FLAGS_log_prefix = true;  // the prefix should print at the beginning of each row
        FLAGS_logbufsecs = 0;  // output the cache immediately, no cache
        FLAGS_max_log_size = 10;  // set the maximum size of the glog file (unit is MB)
        FLAGS_stop_logging_if_full_disk = true;  // disable the output of glog if the disk is full or the disk has no free space
    }
}
#endif //CHAMELEON_CONFIGURATION_GLOG_HPP
