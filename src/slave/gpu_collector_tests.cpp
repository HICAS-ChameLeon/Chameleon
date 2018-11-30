#include "gpu_collector.hpp"
using namespace chameleon;
int main(){
    GpuCollector gpuCollector;
    gpuCollector.split_gpu_string(gpuCollector.get_gpu_string());

    GPUCollection proto = gpuCollector.get_gpu_proto();
    GpuInfo *inx;

    int num = proto.gpu_infos_size();
    proto.set_gpu_quantity(num);
    cout<<"size : "<<proto.gpu_quantity()<<endl;

    for(int i; i<num; i++){
        inx = proto.mutable_gpu_infos(i);
        cout<<"第"<<i<<"组"<<inx->description()<<endl;
        cout<<"第"<<i<<"组"<<inx->product()<<endl;
        cout<<"第"<<i<<"组"<<inx->vendor()<<endl;
        cout<<"第"<<i<<"组"<<inx->physical_id()<<endl;
        cout<<"第"<<i<<"组"<<inx->bus_info()<<endl;
        cout<<"第"<<i<<"组"<<inx->version()<<endl;
        cout<<"第"<<i<<"组"<<inx->width()<<endl;
        cout<<"第"<<i<<"组"<<inx->clock()<<endl;
        cout<<"第"<<i<<"组"<<inx->capabilities()<<endl;
        cout<<"第"<<i<<"组"<<inx->configuration()<<endl;
        cout<<"第"<<i<<"组"<<inx->resources()<<endl;

    }
}
