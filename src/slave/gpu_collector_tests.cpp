#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <gpu_collector.hpp>
#include <runtime_resources_usage.hpp>

TEST(GPUCollectorTests, GetGpuInfo)
{
    chameleon::GpuCollector gpuCollector;
    GPUCollection proto = *gpuCollector.split_gpu_string();
    GPUInfo *inx;

    cout<<"size : "<<proto.gpu_quantity()<<endl;
    //EXPECT_EQ(1,proto.gpu_quantity());
    for(int i; i<proto.gpu_quantity(); i++){
        inx = proto.mutable_gpu_infos(i);
        cout<<"group["<<i<<"]: "<<inx->description()<<endl;
        cout<<"group["<<i<<"]: "<<inx->product()<<endl;
        cout<<"group["<<i<<"]: "<<inx->vendor()<<endl;
        cout<<"group["<<i<<"]: "<<inx->physical_id()<<endl;
        cout<<"group["<<i<<"]: "<<inx->bus_info()<<endl;
        cout<<"group["<<i<<"]: "<<inx->version()<<endl;
        cout<<"group["<<i<<"]: "<<inx->width()<<endl;
        cout<<"group["<<i<<"]: "<<inx->clock()<<endl;
        cout<<"group["<<i<<"]: "<<inx->capabilities()<<endl;
        cout<<"group["<<i<<"]: "<<inx->configuration()<<endl;
        cout<<"group["<<i<<"]: "<<inx->resources()<<endl;

    }
}

TEST(CPUCollectorTests, GetCpuUsage)
{
    chameleon::RuntimeResourceUsage cpuRuntimeUsage;
    chameleon::RuntimeResourceUsage::CpuOccupy f_cpu,s_cpu;

    //获取cpu使用率信息
    cpuRuntimeUsage.get_cpu_used_info(&f_cpu);
    sleep(1);
    cpuRuntimeUsage.get_cpu_used_info(&s_cpu);
    CPUUsage cpuUsage = *cpuRuntimeUsage.cal_cpu_usage(&f_cpu, &s_cpu);
    printf("cpu used:%4.2f \n", cpuUsage.cpu_used());
}

int main(int argc, char** argv) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}