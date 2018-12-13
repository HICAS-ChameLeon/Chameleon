/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：heldon 764165887@qq.com
 * Date       ：18-12-04
 * Description：Get disk's information
 */

#include <disk_collector.hpp>

namespace chameleon{

    DiskCollector::~DiskCollector() {}

    /*
    * Function name：get_disk_collection
    * Author       ：heldon
    * Date         ：2018-12-04
    * Description  ：To get disk's name,size,type,speed,rest storage,available storage then store them into protobuf
    * Parameter    ：none
    * Return       ：DiskCollection*
    */
    DiskCollection* DiskCollector::get_disk_collection() {
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
        DiskCollection *disk_collection = new DiskCollection();

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
                            //disk_type = blockdevices_array_object_map.at(type_t).as<JSON::String>().value;


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

                            /*read /sys/block/sda/queue/rotational to find disk_type*/
                            Try<string> readstring  = os::read("/sys/block/sda/queue/rotational");
                            disk_type = strings::trim(readstring.get(), "\n");

                            /*Assign to protobuf*/
                            if (disk_name.isSome() && disk_size.isSome() && disk_type.isSome() &&
                                disk_speed.isSome()) {

                                DiskInfo *disk_info = disk_collection->add_disk_infos();

                                disk_info->set_name(disk_name.get());
                                disk_info->set_size(disk_size.get());

                                if(disk_type.get() == "0")
                                    disk_info->set_type(DiskInfo::SSD);
                                else if(disk_type.get() == "1")
                                    disk_info->set_type(DiskInfo::HDD);

                                disk_info->set_disk_speed(disk_speed.get());

                            } else{
                                LOG(ERROR)<< "data lost,please check";
                            }
                        }
                            /*If there isn't a key called "type"*/
                        else
                            continue;
                    }
                }
            }
        }
        disk_collection->set_disk_quantity(disk_collection->disk_infos_size());
        return disk_collection;
    }
}