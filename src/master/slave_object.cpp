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
        m_hostname(hardware_resources_.slave_hostname()),
        m_upid_str("slave@"+m_ip+":"+stringify(upid_.address.port)){
        m_available_cpus = m_hardware_resources.cpu_collection().cpu_quantity();
        m_available_mem=0;
        for(int i=0;i<m_hardware_resources.mem_collection().device_quantity();i++){
            const string mem_size = m_hardware_resources.mem_collection().mem_infos(i).size();
            vector<string> split_vector = strings::split(mem_size," "); // 8192 MB
           try{
               double card_size= boost::lexical_cast<double >(split_vector[0]);
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

     /**
      *
      * @param framework_id
      * @return true: the slaveObject actually has the framework specified by the fraemework_id, otherwise return false
      */
     bool SlaveObject::restore_resource_of_framework(const string &framework_id) {
        if(m_framework_resources.count(framework_id)){
            const ResourcesOfFramework& resource_framework = m_framework_resources.at(framework_id);
            m_available_cpus +=resource_framework.m_consumped_cpus;
            m_available_mem+= resource_framework.m_consumped_mem;
            m_available_disk += resource_framework.m_consumped_disk;
            m_framework_resources.erase(framework_id);
            return true;
        }
        return false;
    }


    mesos::Offer* SlaveObject::construct_a_offer(const string& offer_id, const mesos::FrameworkID &framework_id) {
        mesos::Offer *offer = new mesos::Offer();
        // cpus
        mesos::Resource *cpu_resource = new mesos::Resource();
        cpu_resource->set_name("cpus");
        cpu_resource->set_type(mesos::Value_Type_SCALAR);
        mesos::Value_Scalar *cpu_scalar = new mesos::Value_Scalar();
        cpu_scalar->set_value(m_available_cpus);
        cpu_resource->mutable_scalar()->CopyFrom(*cpu_scalar);
        offer->add_resources()->MergeFrom(*cpu_resource);

        // memory
        mesos::Resource *mem_resource = new mesos::Resource();
        mem_resource->set_name("mem");
        mem_resource->set_type(mesos::Value_Type_SCALAR);
        mesos::Value_Scalar *mem_scalar = new mesos::Value_Scalar();
        mem_scalar->set_value(m_available_mem);
        mem_resource->mutable_scalar()->CopyFrom(*mem_scalar);
        offer->add_resources()->MergeFrom(*mem_resource);

        // port
        mesos::Resource *port_resource = new mesos::Resource();
        port_resource->set_name("ports");
        port_resource->set_type(mesos::Value_Type_RANGES);

        mesos::Value_Range *port_range = port_resource->mutable_ranges()->add_range();
        port_range->set_begin(31000);
        port_range->set_end(32000);
        offer->add_resources()->MergeFrom(*port_resource);

        mesos::OfferID offerId;
        offerId.set_value(offer_id);
        offer->mutable_id()->CopyFrom(offerId);

        offer->mutable_framework_id()->MergeFrom(framework_id);

        mesos::SlaveID *slaveID = new mesos::SlaveID();
        slaveID->set_value(m_uuid);
        LOG(INFO)<<m_uuid;
        offer->mutable_slave_id()->MergeFrom(*slaveID);

        offer->set_hostname(m_hostname);
        return offer;
    }
}
