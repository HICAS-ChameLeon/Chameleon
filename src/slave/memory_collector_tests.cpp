/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：marcie 316187187@qq.com
 * Date       ：18-11-30
 * Description：Test of get memory's information
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "memory_collector.hpp"
#include "runtime_resources_usage.hpp"

/*
 * Function name：main
 * Author       ：marcie
 * Date         ：2018-11-30
 * Description  ：To test whether memory information can print correctly
 * Output       :memory information
 * Parameter    ：none
 * Return       ：0
 */
TEST(MemoryCollection, getMemInfo){
    chameleon::MemoryCollector memoryCollector;
    memoryCollector.get_info_rows();
}

TEST(MemoryCollection, SelectMemInfo){
    chameleon::MemoryCollector memoryCollector;
    memoryCollector.select_meminfo();
}

TEST(MemoryCollection, ShowMemInfo){
    chameleon::MemoryCollector memoryCollector;
    memoryCollector.show_meminfo();
}

TEST(MemoryCollection, SelectMemUsage){
    chameleon::RuntimeResourceUsage memoryCollector;
    memoryCollector.select_memusage();
}

TEST(MemoryCollection, ShowMemUsage){
    chameleon::RuntimeResourceUsage memoryCollector;
    memoryCollector.show_memusage();
}

int main(int argc, char** argv) {
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
