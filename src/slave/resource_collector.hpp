/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-30
 * Description：ResourceCollector
 */
#ifndef CHAMELEON_RESOURCECOLLECTOR_HPP
#define CHAMELEON_RESOURCECOLLECTOR_HPP

// C++ 11 dependencies
#include <iostream>
#include <memory>
#include <vector>

#include <hardware_resource.pb.h>

#include <disk_collector.hpp>

using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::make_shared;

namespace chameleon {
    class ResourceCollector {
    public:
        explicit ResourceCollector(){
            msp_disk = make_shared<DiskCollector>(DiskCollector());
        }

        virtual ~ResourceCollector(){

        }

        HardwareResourcesMessage& collect_hardware_resources(){

            // disk collector
            vector<DiskInfo> local_diskinfos = msp_disk->get_disk_collection();
            for(auto it = local_diskinfos.begin();it!=local_diskinfos.end();it++){
                cout << "name: "                    << it->name()
                       << " size: "                   << it->size()
                       << " type: "                   << it->type()           <<'\n'
                       <<"Timing disk reads speed = " << it->disk_speed()     << " MB/s " << '\n'
                       << "Disk_free = "              << it->disk_free()      << " GB"    << '\n'
                       << "Disk_available = "         << it->disk_available() << " GB"    << '\n';
            }
//
//            std::cout<<msp_disk.get()<<std::endl;
//            int a = 4;
        }

    private:
        shared_ptr<DiskCollector> msp_disk;

    };


}


#endif //CHAMELEON_RESOURCECOLLECTOR_HPP
