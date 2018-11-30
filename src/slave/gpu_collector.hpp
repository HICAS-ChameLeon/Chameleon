#ifndef LIBPROCESS_START_GPUCOLLECTOR_H
#define LIBPROCESS_START_GPUCOLLECTOR_H

#include <string>

#include <process/subprocess.hpp>
#include <process/future.hpp>
#include <process/io.hpp>

#include "hardware_resource.pb.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;
using namespace process;
namespace chameleon {
/*
 * className：GpuCollector
 * date：18/11/26
 * author：ZhangYixin 1968959287@qq.com
 * description： Collecting GPU information by executing commands.
 */
    class GpuCollector {
    private:
        string m_gpu_info;
        GPUCollection m_gpu_proto;
    public:
        string get_gpu_string();

        void split_gpu_string(string m_gpu_info);

        GPUCollection get_gpu_proto();
    };
}

#endif //LIBPROCESS_START_GPUCOLLECTOR_H
