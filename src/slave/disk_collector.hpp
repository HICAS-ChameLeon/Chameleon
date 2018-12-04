/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：heldon 764165887@qq.com
 * Date       ：18-11-30
 * Description：Get disk's information
 */

//C++11 dependencies
#include <iostream>
#include <string>

//stout dependencies
#include <stout/path.hpp>
#include <stout/try.hpp>
#include <stout/strings.hpp>
#include <stout/jsonify.hpp>
#include <stout/protobuf.hpp>

//libprocess dependencies
#include <process/subprocess.hpp>
#include <process/protobuf.hpp>
#include <process/io.hpp>

//protobuf dependencies
#include <hardware_resource.pb.h>

namespace io = process::io;

using process::Subprocess;
using process::subprocess;

using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;

namespace chameleon {

    /*
    * Class name ：DiskCollector
    * Author     ：heldon  764165887@qq.com
    * Date       ：18-11-30
    * Description：A Class to get Disks' information
    */

    class DiskCollector {
    public:
        /*
        * Function name：get_disk_collection
        * Author       ：heldon
        * Date         ：2018-11-30
        * Description  ：To get disk's name,size,type,speed,rest storage,available storage then store them into protobuf
        * Parameter    ：none
        * Return       ：DiskCollection
        */
        DiskCollection* get_disk_collection();


        /*
       * Function name：operator<<
       * Author       ：heldon
       * Date         ：2018-11-30
       * Description  ：Overload the symbol of output
       * Parameter    ：ostream, disk_collector
       * Return       ：ostream
       */
        friend std::ostream &operator<<(std::ostream &stream, DiskCollector *&disk_collector) {

            DiskCollection *disk_collection = disk_collector->get_disk_collection();

            stream << "disk_quantity:" << disk_collection->disk_quantity() << '\n';
            for (int i = 0; i < disk_collection->disk_infos_size(); i++) {
                DiskInfo *disk_info = disk_collection->mutable_disk_infos(i);
                string disk_type;
                if(disk_info->type() == 0)
                    disk_type = "SSD";
                else if(disk_info->type() == 1)
                    disk_type = "HDD";
                else
                    disk_type = "don't know";

                stream << "name: " << disk_info->name()
                       << " size: " << disk_info->size()
                       << " type: " << disk_type << '\n'
                       << "Timing disk reads speed = " << disk_info->disk_speed() << " MB/s " << '\n';
            }
            return stream;
        }
    };
}
