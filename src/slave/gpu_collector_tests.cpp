#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <gpu_collector.hpp>

TEST(GPUCollectorTests, GetGpuInfo)
{
    chameleon::GpuCollector gpuCollector;
    GPUCollection proto = *gpuCollector.split_gpu_string();
    GpuInfo *inx;

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

int main(int argc, char** argv) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}