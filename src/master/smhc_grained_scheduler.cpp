/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：20119-7-14
 * Description： smhc_grained_scheduler.cpp
 */

#include "smhc_grained_scheduler.hpp"

namespace chameleon {

//    SMHCGrainedScheduler::SMHCGrainedScheduler() : SchedulerInterface("SMHCGrained") {
//    }
//
//    SMHCGrainedScheduler::~SMHCGrainedScheduler(){
//
//    }

    void SMHCGrainedScheduler::construct_offers(mesos::internal::ResourceOffersMessage &resource_offers_message,
                                                const mesos::FrameworkID &framework_id,
                                                const unordered_map<string, shared_ptr<chameleon::SlaveObject>> &m_slave_objects) {

        /**
         * SMHC_Scheduling   :   Scheduling algorithm based on fine-grained resources, it's approximate characterization of the computing power of the Ith server
         * Algorithm formula : F(x , y) = (a + b + c) * X + d * Y + e
         *                    a:CPU model; b:CPU MHz; c:L3 cache; d:Memory Speed; e: Disk Speed
         *                    X is the ratio of the server CPU cores to the total CPU cores in the cluster
         *                    Y is the ratio of the server Memory to the total Memory in the cluster
         * */
        int a = 0, c = 0 , d = 0, e = 0;
        double b = 0;
        double F = 0;

        int x_total = 0;
        double y_total = 0;

        LOG(INFO) << "smhc-grained scheduling";

        for (auto it = m_slave_objects.begin(); it != m_slave_objects.end(); it++) {
            shared_ptr<SlaveObject> slave = it->second;
            x_total = slave->m_hardware_resources.cpu_collection().cpu_quantity() + x_total ;
            for (auto i = slave->m_hardware_resources.mem_collection().mem_infos().begin();
                 i != slave->m_hardware_resources.mem_collection().mem_infos().end();
                 i++){
                vector<string> vec = strings::split(i->size(), "MB");
                y_total = strtod(vec.front().data(), NULL) + y_total;
            }
        }

        LOG(INFO) << "Have " << x_total << " CPU Cores && " << y_total << " Memory ";

        double result = 0;
        shared_ptr<SlaveObject> result_slave;
//            result_slave = make_shared<SlaveObject>();

        for (auto it = m_slave_objects.begin(); it != m_slave_objects.end(); it++) {

            double x_slave = 0;
            double y_slave = 0;
            double rate = 0.6;

            shared_ptr<SlaveObject> slave = it->second;

            const MemInfo &memInfo = slave->m_hardware_resources.mem_collection().mem_infos(0);
            const CPUInfo &cpuInfo = slave->m_hardware_resources.cpu_collection().cpu_infos(0);
            const DiskInfo &diskInfo = slave->m_hardware_resources.disk_collection().disk_infos(0);

            //CPU Model
            string m_model = cpuInfo.modelname();
            vector<string> vec_modul = strings::split(m_model, " ");
            vector<string> vec2_modul = strings::split(vec_modul[3], "-");

            //CPU L3 Cache
            string m_cpucache = cpuInfo.l3cache();
            vector<string> vec_cpu = strings::split(m_cpucache, "K");

            //Memory Speed
            string mem_speed = memInfo.speed();
            vector<string> vec_mem = strings::split(mem_speed, " ");

            //Disk Speed
            string m_diskspeed = diskInfo.disk_speed();


            string a_cpu_model = vec2_modul[0];                       //CPU Model
            if (a_cpu_model == "i3") {
                a = SMHC::a1;
            } else if (a_cpu_model == "i5") {
                a = SMHC::a2;
            } else if (a_cpu_model == "i7") {
                a = SMHC::a3;
            } else if (m_model.find("E5") != string::npos) {
                a = SMHC::a4;
            }


            int b_cpu_speed = cpuInfo.cpumhz();                       // cpu clock speed
            if ((int(b_cpu_speed / 1000)) >= 0 && (int(b_cpu_speed / 1000)) < 2) {
                b = SMHC::b1;
            } else if ((int(b_cpu_speed / 1000)) >= 2 && (int(b_cpu_speed / 1000)) < 3) {
                b = SMHC::b2;
            } else if ((int(b_cpu_speed / 1000)) >= 3) {
                b = SMHC::b3;
            }

            int c_cpu_l3 = stoi(vec_mem[0]) / 1000;                   //CPU L3 Cache
            if (c_cpu_l3 > 0 && c_cpu_l3 <= 10) {
                c = SMHC::c1;
            } else if (c_cpu_l3 > 10 && c_cpu_l3 <= 20) {
                c = SMHC::c2;
            } else if (c_cpu_l3 > 20 && c_cpu_l3 <= 30) {
                c = SMHC::c3;
            } else if (c_cpu_l3 > 30) {
                c = SMHC::c4;
            }

            int d_mem_speed = stoi(vec_mem[0]);                      // memory speed
            if (d_mem_speed < 2000) {
                d = SMHC::d1;
            } else if (d_mem_speed >= 2000) {
                d = SMHC::d2;
            }

            int e_disk_speed = stoi(m_diskspeed);                    //Disk speed
            if (e_disk_speed < 100) {
                e = SMHC::e1;
            } else if (e_disk_speed >= 100) {
                e = SMHC::e2;
            }

            double slave_memory = 0;
            for (auto i = slave->m_hardware_resources.mem_collection().mem_infos().begin();
                 i != slave->m_hardware_resources.mem_collection().mem_infos().end(); i++){
                vector<string> vec_slave = strings::split(i->size(), "MB");
                slave_memory = strtod(vec_slave.front().data(), NULL) + slave_memory;
            }

            x_slave = slave->m_available_cpus / x_total;
            y_slave = slave_memory / y_total;

            LOG(INFO) << "THis slave have " << slave->m_available_cpus << " CPU rate is " << x_slave
                      << " && have " << memInfo.size().data() << " Memory rate is " << y_slave;

            LOG(INFO) << "Result_slave is " << slave->m_ip;

            F = rate * (a + b + c) * x_slave + (1 - rate) * d * y_slave + e;

            if (F > result) {
                result = F;
                result_slave = slave;
            }
        }
        mesos::OfferID offer_id = new_offer_id();
        mesos::Offer *offer = result_slave->construct_a_offer(offer_id.value(), framework_id);
        resource_offers_message.add_offers()->MergeFrom(*offer);
        LOG(INFO) << offer->slave_id().value();
        m_offers[offer->slave_id().value()] = offer->id().value();
        resource_offers_message.add_pids(result_slave->m_upid_str);
    }

}