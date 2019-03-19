/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：weiguow 2573777501@qq.com
 * Date       ：19-3-16
 * Description：one scheduling algorithms are provided
 *              (1) Based on mixed judgment of multiple resources
 */
namespace chameleon {
    Try<string> MetricScheduler::scheduler(unordered_map <string, HardwareResourcesMessage> hardware_resource,
            unordered_map <string, RuntimeResourcesMessage> runtime_resource) {
        for (auto it = hardware_resource.begin() ; it != hardware_resource.end() ; it++) {

        }
    }
}