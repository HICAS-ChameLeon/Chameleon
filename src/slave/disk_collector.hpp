/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：heldon 764165887@qq.com
 * Date       ：18-11-30
 * Description：Get disk's information
 */

//C++11 dependencies
#include <iostream>
#include <sys/statfs.h>
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

/*
 * Class name ：disk_collector
 * Author     ：heldon  764165887@qq.com
 * Date       ：18-11-30
 * Description：A Class to get Disks' information
 */

class DiskCollector{
public:
       /*
       * Function name：get_disk_collection
       * Author       ：heldon
       * Date         ：2018-11-30
       * Description  ：To get disk's name,size,type,speed,rest storage,available storage then store them into protobuf
       * Parameter    ：none
       * Return       ：DiskCollection
       */
        DiskCollection get_disk_collection() {
            Option<string> disk_name;  /*disk's name*/
            Option<string> disk_size;  /*disk's size*/
            Option<string> disk_type;  /*disk's type*/
            Option<string> disk_speed; /*disk's speed*/
            Option<string> disk_free;  /*the rest of storage space*/
            Option<string> disk_available; /*disk's available storage space*/

            /*Use lsblk -J command to get disk's name,size,type by JSON format*/
            Try<Subprocess> lsblk_process = subprocess(
                    "lsblk -J",
                    Subprocess::FD(STDIN_FILENO),
                    Subprocess::PIPE(),
                    Subprocess::FD(STDERR_FILENO));

            process::Future<string> lsblk_get = io::read(lsblk_process.get().out().get());
            string lsblk_string = lsblk_get.get();

            /*Change string to JSON Object */
            Try<JSON::Value> lsblk_value = JSON::parse(lsblk_string);
            JSON::Object lsblk_json_object = lsblk_value->as<JSON::Object>();

            /*Define constant of name,size,type */
            const string name_t = "name";
            const string size_t = "size";
            const string type_t = "type";
            DiskCollection disk_collection;
            //std::vector<DiskInfo> disk_collection_vector;

            /*Get JSON Object's array which called "blockdevices"*/
            Try<JSON::Value> json_object_blockdevices = lsblk_json_object.values["blockdevices"];
            if (json_object_blockdevices.isSome()) {
                JSON::Value blockdevices = json_object_blockdevices.get();
                if (blockdevices.is<JSON::Array>()) {
                    std::vector<JSON::Value> blockdevices_array = blockdevices.as<JSON::Array>().values;
                    /*Iterate through the array called blockdevices*/
                    for (int i = 0; i < blockdevices_array.size(); i++) {
                        JSON::Object blockdevices_array_object = blockdevices_array.at(i).as<JSON::Object>();
                        /*Get thr map from the array*/
                        std::map<string, JSON::Value> blockdevices_array_object_map = blockdevices_array_object.values;
                        /*Determine whether there is a key called "type" in the map*/
                        if (blockdevices_array_object_map.count(type_t)) {
                            /*Determin whether there is a value called "disk" in the map*/
                            if (blockdevices_array_object_map[type_t].as<JSON::String>().value == "disk") {

                                /*Assign value to member variables*/
                                disk_name = blockdevices_array_object_map.at(name_t).as<JSON::String>().value;
                                disk_size = blockdevices_array_object_map.at(size_t).as<JSON::String>().value;
                                disk_type = blockdevices_array_object_map.at(type_t).as<JSON::String>().value;


                                /*Use hdparm command to get current speed of disk*/
                                string hdparm_command = "sudo -S hdparm -t /dev/" + disk_name.get();
                                Try<Subprocess> hdparm_process = subprocess(
                                        hdparm_command,
                                        Subprocess::FD(STDIN_FILENO),
                                        Subprocess::PIPE(),
                                        Subprocess::FD(STDERR_FILENO));

                                process::Future<string> hdparm_get = io::read(hdparm_process.get().out().get());
                                string hdparm_string = hdparm_get.get();
                                /*Tokensize the string as blank space*/
                                std::vector<string> hdparm_token = strings::tokenize(hdparm_string, " ");
                                /*The second-to-last is disk's speed
                                  Assign value to variable*/
                                disk_speed = hdparm_token.at(hdparm_token.size() - 2);

                                /*Use statfs to get disk's rest storage,available storage*/
                                struct statfs diskInfo;
                                /*Find the information under the path "/home" */
                                statfs("/home", &diskInfo);

                                /*The number of bytes contained in each block*/
                                unsigned long long blocksize = diskInfo.f_bsize;
                                /*The total number of bytes, f_blocks is the number of blocks*/
                                unsigned long long totalsize = blocksize * diskInfo.f_blocks;
                                /*The size of the remaining space*/
                                unsigned long long freeDisk = diskInfo.f_bfree * blocksize;
                                /*The size of the avaliable space*/
                                unsigned long long availableDisk = diskInfo.f_bavail * blocksize;

                                /*Transfer the GB unit and assign to variables*/
                                disk_free = stringify(freeDisk >> 30);
                                disk_available = stringify(availableDisk >> 30);

                                /*Assign to protobuf*/
                                if (disk_name.isSome() && disk_size.isSome() && disk_type.isSome() && disk_speed.isSome() &&
                                    disk_free.isSome() && disk_available.isSome()) {

                                    DiskInfo *disk_info = disk_collection.add_disk_infos();

                                    disk_info->set_name(disk_name.get());
                                    disk_info->set_size(disk_size.get());
                                    disk_info->set_type(disk_type.get());
                                    disk_info->set_disk_speed(disk_speed.get());
                                    disk_info->set_disk_free(disk_free.get());
                                    disk_info->set_disk_available(disk_available.get());

                                } else
                                    cout<<"data lost,please check"<<endl;
                            }
                            /*If there isn't a key called "type"*/
                            else
                                continue;
                        }
                    }
                }
            }
            disk_collection.set_disk_quantity(disk_collection.disk_infos_size());
            return disk_collection;
        }

        /*
       * Function name：operator<<
       * Author       ：heldon
       * Date         ：2018-11-30
       * Description  ：Overload the symbol of output
       * Parameter    ：ostream, disk_collector
       * Return       ：ostream
       */
        friend std::ostream &operator<<(std::ostream &stream, DiskCollector *&disk_collector) {

            auto disk_collection = disk_collector->get_disk_collection();

            stream << "disk_quantity:" <<disk_collection.disk_quantity() << '\n';
            for(int i = 0;i < disk_collection.disk_infos_size(); i++){
                auto disk_info = disk_collection.mutable_disk_infos(i);
                stream << "name: "                     << disk_info->name()
                       << " size: "                    << disk_info->size()
                       << " type: "                    << disk_info->type()            << '\n'
                       << "Timing disk reads speed = " << disk_info->disk_speed()      << " MB/s " << '\n'
                       << "Disk_free = "               << disk_info->disk_free()       << " GB"    << '\n'
                       << "Disk_available = "          << disk_info->disk_available()  << " GB"    << '\n';
            }
            return stream;
        }
};
