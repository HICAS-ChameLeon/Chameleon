//
// Created by lemaker on 18-12-4.
//

#include <runtime_resources_usage.hpp>
namespace chameleon{
    /*
    * Function name：get_disk_usage
    * Author       ：heldon
    * Date         ：2018-12-04
    * Description  ：get disk usage
    * Parameter    ：none
    * Return       ：DiskUsage*
    */
    DiskUsage* chameleon::RuntimeResourceUsage::get_disk_usage(){
        Option<int64_t> disk_available;
        Option<double > available_percent;

        DiskUsage* disk_usage;

        /*Use statfs to get disk's rest storage,available storage*/
        struct statfs diskInfo;
        /*Find the information under the path "/home" */
        statfs("/home", &diskInfo);

        /*The number of bytes contained in each block*/
        unsigned long long blocksize = diskInfo.f_bsize;
        /*The total number of bytes, f_blocks is the number of blocks*/
        unsigned long long totalsize = blocksize * diskInfo.f_blocks;
        /*The size of the avaliable space*/
        unsigned long long availableDisk = diskInfo.f_bavail * blocksize;

        disk_available = availableDisk;
        double decimal = (double)availableDisk/totalsize;
        available_percent = decimal * 100;

        disk_usage->set_disk_available(disk_available.get());
        disk_usage->set_available_percent(available_percent.get());

        return  disk_usage;
    }

}

