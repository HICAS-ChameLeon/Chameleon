/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：heldon 764165887@qq.com
 * Date       ：18-11-30
 * Description：Test for disk_collector.cpp
 */
#include <string>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <stout/gtest.hpp>

#include <disk_collector.hpp>

using std::shared_ptr;

TEST(DiskCollectorTest, DiskInfo)
{
    chameleon::DiskCollector* disk_collector = new chameleon::DiskCollector();
    DiskCollection* disk_collection = disk_collector->get_disk_collection();

    Option<int> disk_quantity = disk_collection->disk_quantity();
    EXPECT_SOME(disk_quantity);

    for(int i = 0;i < disk_collection->disk_infos_size(); i++){
        Option<DiskInfo*> disk_info = disk_collection->mutable_disk_infos(i);
        EXPECT_SOME(disk_info);
        Option<string> disk_name = disk_info.get()->name();
        EXPECT_SOME(disk_name);
        Option<string> disk_size = disk_info.get()->size();
        EXPECT_SOME(disk_size);
        Option<string> disk_type = disk_info.get()->type();
        EXPECT_SOME(disk_type);
        Option<string> disk_speed = disk_info.get()->disk_speed();
        EXPECT_SOME(disk_speed);
        Option<string> disk_free = disk_info.get()->disk_free();
        EXPECT_SOME(disk_free);
        Option<string> disk_available = disk_info.get()->disk_available();
        EXPECT_SOME(disk_available);
    }

}

int main(int argc, char** argv) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}