#include <gpu_collector.hpp>
using namespace chameleon;
int main(){
    GpuCollector gpuCollector;
    string info = gpuCollector.get_gpu_string();
    gpuCollector.split_gpu_string(info);
    GPUCollection proto = *gpuCollector.get_gpu_proto();
    GpuInfo *inx;

    int num = proto.gpu_infos_size();
    proto.set_gpu_quantity(num);
    cout<<"size : "<<proto.gpu_quantity()<<endl;

    for(int i; i<num; i++){
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
