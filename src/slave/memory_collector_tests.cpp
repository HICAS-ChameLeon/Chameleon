/*
 * Copyright  ：深圳先进研究所异构智能计算体系结构与系统研究中心
 * Author     ：marcie 316187187@qq.com
 * Date       ：18-11-30
 * Description：Test of get memory's information
 */

#include "memory_collector.hpp"

/*
 * Function name：main
 * Author       ：marcie
 * Date         ：2018-11-30
 * Description  ：To test whether memory information can print correctly
 * Output       :memory information
 * Parameter    ：none
 * Return       ：0
 */
int main(){
    chameleon::MemoryCollector memoryCollector;
    memoryCollector.get_dmiinfo_rows();
    memoryCollector.select_meminfo(memoryCollector.m_tokens);
    memoryCollector.show_meminfo(memoryCollector.m_memory_collection);
}