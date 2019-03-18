/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：20119-3-15
 * Description： slave_object
 */

#include "slave_object.hpp"

namespace chameleon {
    SlaveObject::SlaveObject(const UPID &upid_, const HardwareResourcesMessage &hardware_resources_
    ) : m_hardware_resources(hardware_resources_),
        m_upid(upid_),
        m_ip(hardware_resources_.slave_id()),
        m_uuid(hardware_resources_.slave_uuid()),
        m_hostname(hardware_resources_.slave_hostname()){
        m_available_cpus = m_hardware_resources.cpu_collection().cpu_quantity();
        m_available_mem=0;
        for(int i=0;i<m_hardware_resources.mem_collection().device_quantity();i++){
            const string mem_size = m_hardware_resources.mem_collection().mem_infos(i).size();
            vector<string> split_vector = strings::split(mem_size," "); // 8192 MB
           try{
               uint32_t card_size= boost::lexical_cast<uint32_t >(split_vector[0]);
               m_available_mem+=card_size;
           }catch (boost::bad_lexical_cast& e){
               LOG(ERROR)<<mem_size;
               LOG(ERROR)<<e.what();
           }
        }
        m_available_disk =0;
    }

    SlaveObject::~SlaveObject() {

    }

    mesos::Offer SlaveObject::construct_a_offer() {

    }
}
