/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */

#include <super_master_related.pb.h>
#include "master.hpp"

DEFINE_int32(port, 0, "master port");

/*
 * Function name  : ValidateInt
 * Author         : weiguow
 * Date           : 2018-12-13
 * Description    : Determines whether the input parameter is valid
 * Return         : True or False*/
static bool ValidateInt(const char *flagname, gflags::int32 value) {
    if (value >= 0 && value < 32768) {
        return true;
    }
    printf("Invalid value for --%s: %d\n", flagname, (int) value);
    return false;
}

static const bool port_dummyInt = gflags::RegisterFlagValidator(&FLAGS_port, &ValidateInt);

namespace chameleon {

//    Slave::Slave(
//            Master *const _master,
//            const mesos::SlaveInfo &_info,
//            const UPID &_pid)
//            : master(_master),
//              id(_info.id()),
//              info(_info),
//              pid(_pid) {
//
//    }

    void Master::initialize() {

        m_uuid = UUID::random().toString();
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        install<ParticipantInfo>(&Master::register_participant, &ParticipantInfo::hostname);

        install<HardwareResourcesMessage>(&Master::update_hardware_resources);
        //install<mesos::FrameworkInfo>(&Master::change_frameworks);  // wqn changes
        install<JobMessage>(&Master::job_submited);
        install<RuntimeResourcesMessage>(&Master::received_heartbeat);
        install<AcceptRegisteredMessage>(&Master::received_registered_message_from_super_master);

        install<mesos::internal::StatusUpdateMessage>(
                &Master::statusUpdate,
                &mesos::internal::StatusUpdateMessage::update,
                &mesos::internal::StatusUpdateMessage::pid);

        install<mesos::internal::StatusUpdateAcknowledgementMessage>(
                &Master::statusUpdateAcknowledgement,
                &mesos::internal::StatusUpdateAcknowledgementMessage::slave_id,
                &mesos::internal::StatusUpdateAcknowledgementMessage::framework_id,
                &mesos::internal::StatusUpdateAcknowledgementMessage::task_id,
                &mesos::internal::StatusUpdateAcknowledgementMessage::uuid);

        install<SuperMasterControlMessage>(&Master::super_master_control);
        install<TerminatingMasterMessage>(&Master::received_terminating_master_message);

//        install<ReplyShutdownMessage>(&Master::received_reply_shutdown_message,&ReplyShutdownMessage::slave_ip, &ReplyShutdownMessage::is_shutdown);

        /**
         * Function  :  install schedule
         * Author    :  weiguow
         * Date      :  2018-12-27
         * */
        install<mesos::scheduler::Call>(&Master::receive);

        //change two levels to one related
        install("MAKUN",&Master::get_select_master);
//        install<TerminatingMasterMessage>

        // http://172.20.110.228:6060/master/hardware-resources
        route(
                "/hardware-resources",
                "get the topology resources of the whole topology",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    if (!this->m_hardware_resources.empty()) {
                        JSON::Array array;
                        for (auto it = this->m_hardware_resources.begin();
                             it != this->m_hardware_resources.end(); it++) {
                            array.values.push_back(it->second);
                        }
                        result.values["quantity"] = array.values.size();
                        result.values["content"] = array;
                    } else {
                        result.values["quantity"] = 0;
                        result.values["content"] = JSON::Object();
                    }
                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
                });

        // http://172.20.110.228:6060/master/runtime-resources
        route(
                "/runtime-resources",
                "get the runtime resources of the whole topology",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    if (!this->m_runtime_resources.empty()) {
                        JSON::Array array;
                        for (auto it = this->m_runtime_resources.begin();
                             it != this->m_runtime_resources.end(); it++) {
                            array.values.push_back(it->second);
                        }
                        result.values["quantity"] = array.values.size();
                        result.values["content"] = array;
                    } else {
                        result.values["quantity"] = 0;
                        result.values["content"] = JSON::Object();
                    }
                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
                });

        // http://172.20.110.228.6060/master/frameworks
        route(
                "/frameworks",
                "get the frameworks of the whole topology",
                [this](Request request) {
                    //JSON::Object result = JSON::Object();
                    JSON::Object result = JSON::protobuf(m_frameworkInfo);
                    OK ok_response(stringify(result));
                    //OK ok_id_response(stringify(id));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    //ok_id_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;

                });

        route(
                "/frameworksID",
                "get the frameworks of the whole topology",
                [this](Request request) {
                    //JSON::Object result = JSON::Object();
                    JSON::Object result = JSON::protobuf(m_frameworkID);
                    OK ok_response(stringify(result));
                    //OK ok_id_response(stringify(id));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;

                });


        // http://172.20.110.228:6060/master/stop-cluster
        route(
                "/stop-cluster",
                "try to stop the whole Chameleon cluster",
                [this](Request request) {
                    JSON::Object result = JSON::Object();

                    //send a shutdown message to every slave
                    LOG(INFO) << "stopping the Chameleon cluster, we have " << m_alive_slaves.size()
                              << " slaves to terminate";
                    for (string ip : this->m_alive_slaves) {
                        const UPID current_slave(construct_UPID_string("slave", ip, "6061"));
                        ShutdownMessage m;
                        m.set_master_ip(this->self().id);
                        m.set_slave_ip(ip);
                        send(current_slave, m);
                        LOG(INFO) << self() << "sent a shutdown message to " << current_slave;
                    }

                    result.values["stop"] = "success";

                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
                });


//     install("stop", &MyProcess::stop);
        install("stop", [=](const UPID &from, const string &body) {
            terminate(self());
        });

        // super_master related
        // when we have only one level (i.e. we have no super_master),
        // is_passive = true stands fro that the master is started initiatively.
        is_passive = false;

        m_state = RUNNING;

    }

    /**
    * Function      : getFramework
    * Description   : get framework by frameworkID
    * @param       : frameworkID
    * */
//    Framework* Master::getFramework(const mesos::FrameworkID& frameworkId) const {
//        if (frameworks.registered.contains(frameworkId)) {
//            return frameworks.registered.at(frameworkId);
//        } else {
//            return nullptr;
//        }
//    }

    /**
     * Function    : getOffer
     * Description : get offerinfo by offerID
     * @param      : offerID
     * */
//    mesos::Offer* Master::getOffer(const mesos::OfferID &offerId) const {
//        if (offers.contains(offerId)) {
//            return offers.at(offerId);
//        } else {
//            return nullptr;
//        }
//    }

    /**
      * Function model  :  sprak run on chameleon
      * Author          :  weiguow
      * Date            :  2018-12-27
      * Funtion name    :  receive
      * @param         : UPID &from ,Call &call
      * */
    void Master::receive(const UPID &from, const mesos::scheduler::Call &call) {

        //first call
        if (call.type() == mesos::scheduler::Call::SUBSCRIBE) {
            subscribe(from, call.subscribe());
            return;
        }

        switch (call.type()) {
            case mesos::scheduler::Call::SUBSCRIBE:
                LOG(FATAL) << "Unexpected 'SUBSCRIBE' call";
                break;

            case mesos::scheduler::Call::ACCEPT:
                accept(from, call.accept());
                break;

            case mesos::scheduler::Call::ACKNOWLEDGE: {
                Try<UUID> uuid = UUID::fromBytes(call.acknowledge().uuid());
                if (uuid.isError()) {
                    LOG(INFO) << "Ignoring unknow uuid" << uuid.get();
                    return;
                }
                acknowledge(call.acknowledge());
                break;
            }

            case mesos::scheduler::Call::UNKNOWN:
                LOG(WARNING) << "'UNKNOWN' call";
                break;
        }
    }

    /**
     * Function model  :  sprak run on chameleon
     * Author          :  weiguow
     * Date            :  2018-12-28
     * Funtion name    :  subscribe
     * @param          : UPID &from ,Call::Subscribe &subscribe
     * */
    void Master::subscribe(const UPID &from, const mesos::scheduler::Call::Subscribe &subscribe) {

        mesos::FrameworkInfo frameworkInfo = subscribe.framework_info();

        LOG(INFO) << "Received a subscribe framework "
                  << frameworkInfo.name() << "  at " << from;

        m_frameworkInfo = frameworkInfo;
        this->m_frameworkPID = from;  //scheduler-a8426d29-07c4-4b5b-9dc7-2daf941b4893@172.20.110.77:34803

        mesos::internal::FrameworkRegisteredMessage message;

        mesos::MasterInfo masterInfo;
        masterInfo.set_id("11111111");
        masterInfo.set_ip(self().address.ip.in().get().s_addr);
        masterInfo.set_port(6060);

        std::ostringstream out;
        int64_t nextFrameworkId;
        mesos::FrameworkID *frameworkID = new mesos::FrameworkID();
        out << masterInfo.id() << "-" << std::setw(4)
            << std::setfill('0') << nextFrameworkId++;
        frameworkID->set_value(out.str());
        m_frameworkID = *frameworkID;

        message.mutable_framework_id()->MergeFrom(*frameworkID);
        message.mutable_master_info()->MergeFrom(masterInfo);

        send(from, message);

        LOG(INFO) << "Subscribe framework " << frameworkInfo.name() << " successful !";

//        process::dispatch(self(), &Master::Offer, from);
        const Duration temp_duration = Seconds(20);
        process::delay(temp_duration, self(), &Master::Offer, from);

        return;
    }
//    void Master::subscribe(const UPID &from, const mesos::scheduler::Call::Subscribe &subscribe) {
//
//        mesos::FrameworkInfo frameworkInfo = subscribe.framework_info();
//
//        LOG(INFO) << "Received a subscribe framework "
//                  << frameworkInfo.name() << "  at " << from;
//
//        m_frameworkInfo = frameworkInfo;
//
//        this->m_frameworkPID = from;  //scheduler-a8426d29-07c4-4b5b-9dc7-2daf941b4893@172.20.110.77:34803
//
//        mesos::internal::FrameworkRegisteredMessage message;
//
//        mesos::MasterInfo masterInfo;
//        masterInfo.set_id("11111111");
//        masterInfo.set_ip(self().address.ip.in().get().s_addr);
//        masterInfo.set_port(6060);
//
//        std::ostringstream out;
//        int64_t nextFrameworkId;
//        mesos::FrameworkID *frameworkID = new mesos::FrameworkID();
//        out << masterInfo.id() << "-" << std::setw(4)
//            << std::setfill('0') << nextFrameworkId++;
//        frameworkID->set_value(out.str());
//        m_frameworkID = *frameworkID;
//
//        message.mutable_framework_id()->MergeFrom(*frameworkID);
//        message.mutable_master_info()->MergeFrom(masterInfo);
//
//        send(from, message);
//
//        LOG(INFO) << "Subscribe framework " << frameworkInfo.name() << " successful !";
//
////        process::dispatch(self(), &Master::Offer, from);
//        const Duration temp_duration = Seconds(20);
//        process::delay(temp_duration, self(), &Master::Offer,from);
//
//        return;
//    }

    mesos::Offer *Master::create_a_offer() {
        mesos::Offer *offer = new mesos::Offer();

        // cpus
        mesos::Resource *cpu_resource = new mesos::Resource();
        cpu_resource->set_name("cpus");
        cpu_resource->set_type(mesos::Value_Type_SCALAR);
        mesos::Value_Scalar *cpu_scalar = new mesos::Value_Scalar();
        cpu_scalar->set_value(4.0);
        cpu_resource->mutable_scalar()->CopyFrom(*cpu_scalar);
        offer->add_resources()->MergeFrom(*cpu_resource);

        // memory
        mesos::Resource *mem_resource = new mesos::Resource();
        mem_resource->set_name("mem");
        mem_resource->set_type(mesos::Value_Type_SCALAR);
        mesos::Value_Scalar *mem_scalar = new mesos::Value_Scalar();
        mem_scalar->set_value(1200.0);
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
        offerId.set_value("22222222");
        offer->mutable_id()->CopyFrom(offerId);

        offer->mutable_framework_id()->MergeFrom(m_frameworkID);

        mesos::SlaveID *slaveID = new mesos::SlaveID();
        slaveID->set_value("22222222222");

        offer->mutable_slave_id()->MergeFrom(*slaveID);

//        if(m_alive_slaves.size()>0){
//            offer->set_hostname(*m_alive_slaves.begin());
//        }else{
//            offer->set_hostname(self().address.hostname().get());
//        }
        offer->set_hostname("221b");
        return offer;
    }

    /**
     * Function model  :  spark run on chameleon
     * Author          :  weiguow
     * Date            :  2018-12-28
     * Funtion name    :  Master::offer
     * */
    void Master::Offer(const UPID &from) {
        mesos::Offer *offer = new mesos::Offer();

        // cpus
        mesos::Resource *cpu_resource = new mesos::Resource();
        cpu_resource->set_name("cpus");
        cpu_resource->set_type(mesos::Value_Type_SCALAR);
        mesos::Value_Scalar *cpu_scalar = new mesos::Value_Scalar();
        cpu_scalar->set_value(4.0);
        cpu_resource->mutable_scalar()->CopyFrom(*cpu_scalar);
        offer->add_resources()->MergeFrom(*cpu_resource);

        // memory
        mesos::Resource *mem_resource = new mesos::Resource();
        mem_resource->set_name("mem");
        mem_resource->set_type(mesos::Value_Type_SCALAR);
        mesos::Value_Scalar *mem_scalar = new mesos::Value_Scalar();
        mem_scalar->set_value(1000.0);
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
        offerId.set_value("111111111");
        offer->mutable_id()->CopyFrom(offerId);

        offer->mutable_framework_id()->MergeFrom(m_frameworkID);

        mesos::SlaveID *slaveID = new mesos::SlaveID();
        slaveID->set_value("11111111");

        offer->mutable_slave_id()->MergeFrom(*slaveID);

//        if(m_alive_slaves.size()>0){
//            offer->set_hostname(*m_alive_slaves.begin());
//        }else{
//            offer->set_hostname(self().address.hostname().get());
//        }
        offer->set_hostname("AMD-V");


        mesos::internal::ResourceOffersMessage message;
        message.add_offers()->MergeFrom(*offer);
        message.add_pids("1");

        mesos::Offer *second_offer = create_a_offer();
        message.add_offers()->MergeFrom(*second_offer);
        message.add_pids("2");

        LOG(INFO) << "Sending " << message.offers().size() << " offer to framework "
                  << from;

        send(from, message);

        return;
    }

    /**
    * Function model  :  spark run on chameleon
    * Author          :  weiguow
    * Date            :  2018-12-29
    * Funtion name    :  Master::accept
    * */
    void chameleon::Master::accept(const UPID &from, mesos::scheduler::Call::Accept accept) {
        //judge the operation type
        for (int i = 0; i < accept.operations_size(); ++i) {
            mesos::Offer::Operation *operation = accept.mutable_operations(i);
            if (operation->type() == mesos::Offer::Operation::LAUNCH) {
                if (operation->launch().task_infos().size() > 0) {
                    LOG(INFO) << "Get offer from scheduler ";
                } else {
                    LOG(INFO) << "There is no task to run";
                }
            } else if (operation->type() == mesos::Offer::Operation::LAUNCH_GROUP) {
                const mesos::ExecutorInfo &executor = operation->launch_group().executor();
                mesos::TaskGroupInfo *taskGroup = operation->mutable_launch_group()->mutable_task_group();
                for (int j = 0; j < taskGroup->tasks().size(); ++j) {
                    mesos::TaskInfo *task = taskGroup->mutable_tasks(j);
                    if (!task->has_executor()) {
                        task->mutable_executor()->CopyFrom(executor);
                    }
                }
            }
        }

        vector<mesos::Offer::Operation> operations;

        foreach (const mesos::Offer::Operation &operation, accept.operations()) {
            switch (operation.type()) {
                case mesos::Offer::Operation::LAUNCH: {
                    mesos::Offer::Operation _operation;

                    _operation.set_type(mesos::Offer::Operation::LAUNCH);

                    foreach (const mesos::TaskInfo &task, operation.launch().task_infos()) {

//                        for (auto it = this->m_alive_slaves.begin(); it != this->m_alive_slaves.end(); it++) {
//                            m_slavePID = "slave@" + stringify(*it) + ":6061";
//
//                        }
                        string cur_slavePID = "slave@";
                        if (task.slave_id().value() == "11111111") {
                            cur_slavePID.append("172.20.110.228:6061");
                        } else {
                            cur_slavePID.append("172.20.110.53:6061");
                        }
                        mesos::TaskInfo task_(task);

                        LOG(INFO) << "Sending task to slave " << cur_slavePID; //slave(1)@172.20.110.152:5051

                        mesos::internal::RunTaskMessage message;
                        message.mutable_framework()->MergeFrom(m_frameworkInfo);
                        message.set_pid(from);
                        message.mutable_task()->MergeFrom(task_);
                        message.mutable_framework_id()->MergeFrom(m_frameworkID);

                        send(cur_slavePID, message);

                        _operation.mutable_launch()->add_task_infos()->CopyFrom(task);
                    }
                    break;
                }
                case mesos::Offer::Operation::UNKNOWN: {
                    LOG(WARNING) << "Ignoring unknown offer operation";
                    break;
                }

            }
        }
    }

    /**
     * Function     : statusUpdate
     * Author       : weiguow
     * Date         : 2019-1-10
     * Description  : get statusUpdate message from slave and send it to framework
     * */
    void Master::statusUpdate(mesos::internal::StatusUpdate update, const UPID &pid) {
        LOG(INFO) << "Status update " << update.status().state()
                  << " of framework " << update.framework_id().value()
                  << " from agent " << update.slave_id().value();

        if (update.has_uuid()) {
            update.mutable_status()->set_uuid(update.uuid());
        }

        if (update.has_framework_id()) {
            LOG(INFO) << "Forwarding status update " << update.status().state()
                      << " of framework " << update.framework_id().value();

            mesos::internal::StatusUpdateMessage message;
            message.mutable_update()->MergeFrom(update);
            message.set_pid(pid);   //this pid is slavePID
            // m_frameworkPID scheduler-26009ec4-1787-446d-916f-e32fd9baa26a@172.20.110.77:36297;
            send(m_frameworkPID, message);
        }
    }

    /**
     * Function     : statusUpdateAcknowledge
     * Author       : weiguow
     * Date         : 2019-1-10
     * Description  : get statusUpdateAcknowledge message from  and send it to slave
     * */
    void Master::statusUpdateAcknowledgement(
            const UPID &from,
            const mesos::SlaveID &slaveId,
            const mesos::FrameworkID &frameworkId,
            const mesos::TaskID &taskId,
            const string &uuid) {
        LOG(INFO) << "statusUpdateAcknowledgement from " << from;
        mesos::scheduler::Call::Acknowledge message;
        message.mutable_slave_id()->CopyFrom(slaveId);
        message.mutable_task_id()->CopyFrom(taskId);
        message.set_uuid(uuid);

        acknowledge(message);
    }

    /**
    * Function     : acknowledge
    * Author       : weiguow
    * Date         : 2019-1-10
    * Description  : send StatusUpdateAcknowledgementMessage message to
     * slave make sure the status
    * */
    void Master::acknowledge(const mesos::scheduler::Call::Acknowledge &acknowledge) {
        const mesos::SlaveID &slaveId = acknowledge.slave_id();
        const mesos::TaskID &taskId = acknowledge.task_id();
        const UUID uuid = UUID::fromBytes(acknowledge.uuid()).get();

        mesos::internal::StatusUpdateAcknowledgementMessage message;
        message.mutable_slave_id()->CopyFrom(slaveId);

        message.mutable_framework_id()->MergeFrom(m_frameworkID);
        message.mutable_task_id()->CopyFrom(taskId);
        message.set_uuid(uuid.toBytes());

        LOG(INFO) << "Processing ACKNOWLEDGE call " << uuid << " for task " << taskId.value()
                  << " of framework " << m_frameworkInfo.name() << " on agent " << slaveId.value();
        send(m_slavePID, message);

    }

    void Master::register_participant(const string &hostname) {
        DLOG(INFO) << "master receive register message from " << hostname;
    }

    void Master::update_hardware_resources(const UPID &from,
                                           const HardwareResourcesMessage &hardware_resources_message) {
        DLOG(INFO) << "enter update_hardware_resources";

        auto slaveid = hardware_resources_message.slave_id();
        if (m_hardware_resources.find(slaveid) == m_hardware_resources.end()) {
            JSON::Object object = JSON::protobuf(hardware_resources_message);
//                string object_str = stringify(object);
//                DLOG(INFO) << object_str;
            m_hardware_resources.insert({slaveid, object});
            m_proto_hardware_resources.insert({slaveid, hardware_resources_message});
            m_alive_slaves.insert(slaveid);
        }
    }

//    void Master::change_frameworks(const UPID &from, const mesos::FrameworkInfo &frameworkInfo) {
//        DLOG(INFO)<<"change protobuf message to JSON";
//        auto frameworkid = frameworkInfo.;
//        JSON::Object framework_result = JSON::protobuf(frameworkInfo);
//        //string object_str = stringify(object);
//        //LOG(INFO)<<object_str;
//        // m_json_frameworkInfo
//
//    }


//    void Master::job_submited(const UPID &from, const JobMessage &job_message) {
//        LOG(INFO) << "got a job from " << from;
//        send(*msp_spark_master, job_message);
//        LOG(INFO) << "sent the job to the test master 172.20.110.228 successfully!";
//        JobMessage slave_job_message;
//        slave_job_message.CopyFrom(job_message);
//        slave_job_message.set_master_ip("172.20.110.228");
//        slave_job_message.set_is_master(false);
//        LOG(INFO) << "slave_job_message.is_master = " << slave_job_message.is_master();
//        send(*msp_spark_slave, slave_job_message);
//        LOG(INFO) << "sent the job to the test slave 172.20.110.79 successfully!";
//    }

    void Master::job_submited(const UPID &from, const JobMessage &job_message) {
        LOG(INFO) << "got a job from " << from;

        JobMessage slave_job_message;
        slave_job_message.CopyFrom(job_message);
        slave_job_message.set_master_ip("172.20.110.228");
        slave_job_message.set_is_master(false);

        // find the best machine whose sum usage of cpu and memory is the lowest.
        Try<string> best_machine = find_min_cpu_and_memory_rates();
        if (best_machine.isError()) {
            LOG(FATAL) << " cannot find a appropriate machine to run the job!";
            return;
        }
        string str_spark_slave = "slave@";
        str_spark_slave.append(best_machine.get());
        str_spark_slave.append(":6061");
        UPID spark_slave(str_spark_slave);
        LOG(INFO) << "slave_job_message.is_master = " << slave_job_message.is_master();
        send(spark_slave, slave_job_message);
        LOG(INFO) << "sent the job to the test slave " << str_spark_slave << " successfully!";
    }

    void Master::received_heartbeat(const UPID &slave, const RuntimeResourcesMessage &runtime_resouces_message) {
        LOG(INFO) << "received a heartbeat message from " << slave;
        auto slave_id = runtime_resouces_message.slave_id();
        m_runtime_resources[slave_id] = JSON::protobuf(runtime_resouces_message);
        m_proto_runtime_resources[slave_id] = runtime_resouces_message;
        //add insert slave_id to send new master message to slave
        m_alive_slaves.insert(slave_id);
    }

    Try<string> Master::find_min_cpu_and_memory_rates() {
        double min_sum_rate = 100.0;
        string res = "";
        for (auto it = m_proto_runtime_resources.begin(); it != m_proto_runtime_resources.end(); it++) {
            double cur_cpu_rate;
            double cur_mem_rate;
            auto cur_message = it->second;
            cur_cpu_rate = cur_message.cpu_usage().cpu_used() * 0.01;
            cur_mem_rate = static_cast<double>(cur_message.mem_usage().mem_available()) /
                           static_cast<double >(cur_message.mem_usage().mem_total());
            double cur_sum_rate = 50 * cur_cpu_rate + 50 * cur_mem_rate;
            LOG(INFO) << it->first << " cpu usage is " << cur_cpu_rate << " memory usage is " << cur_mem_rate;
            LOG(INFO) << it->first << " cur_sum_rate is " << cur_sum_rate;
            if (min_sum_rate > cur_sum_rate) {
                min_sum_rate = cur_sum_rate;
                res = it->first;
            }
        }
        if (res.empty()) {
            LOG(ERROR) << " calculate the best machine to schedule the new job failed!";
            return Error("The whole cluster has no machine");
        }
        return res;
    }

    void Master::received_reply_shutdown_message(const string &ip, const bool &is_shutdown) {
        if (is_shutdown) {
            for (auto it = m_alive_slaves.begin(); it != m_alive_slaves.end(); it++) {
                if (*it == ip) {
                    m_alive_slaves.erase(it);
                }
            }
            if (m_hardware_resources.count(ip)) {
                m_hardware_resources.erase(ip);
            }
            if (m_runtime_resources.count(ip)) {
                m_runtime_resources.erase(ip);
            }
            if (m_proto_runtime_resources.count(ip)) {
                m_proto_runtime_resources.erase(ip);
            }
            LOG(INFO) << "successfully shutdown a slave " << ip;
        }
    }

    // super_master related
    void Master::super_master_control(const UPID &super_master,
                                      const SuperMasterControlMessage &super_master_control_message) {
        LOG(INFO) << " get a super_master_control_message from super_master" << super_master;
        LOG(INFO) << " passive in super_master_control_message is " << super_master_control_message.passive();

        // change current status to REGISTERRING to register from supermaster.
        m_state = REGISTERING;
        is_passive = super_master_control_message.passive();


        MasterRegisteredMessage *master_registered_message = new MasterRegisteredMessage();
        master_registered_message->set_master_id(stringify(self().address.ip));
        master_registered_message->set_master_uuid(m_uuid);
        master_registered_message->set_status(MasterRegisteredMessage_Status_FIRST_REGISTERING);
        send(super_master, *master_registered_message);
        delete master_registered_message;
        LOG(INFO) << " send a master_registered_message to " << super_master;

        if (is_passive) {
            // is_passive = true means the master was evoked by a super_master,
            // so in super_master_related.proto at line 30 repeated SlavesInfoControlledByMaster my_slaves=4 is not empty
            for (auto &slave_info:super_master_control_message.my_slaves()) {
                UPID slave_upid("slave@" + slave_info.ip() + ":" + slave_info.port());
                ReregisterMasterMessage *register_message = new ReregisterMasterMessage();
                register_message->set_port("6060");
                register_message->set_master_ip(stringify(self().address.ip));
                register_message->set_slave_ip(slave_info.ip());

                send(slave_upid, *register_message);
                LOG(INFO) << "sent a ReregisterMasterMessage to slave " << slave_info.ip();
                delete register_message;
            }
        }

    }


    void Master::received_registered_message_from_super_master(const UPID &super_master,
                                                               const AcceptRegisteredMessage &message) {
        LOG(INFO) << "get a AcceptRegisteredMessage from super_master" << super_master;
        if (message.status() == AcceptRegisteredMessage_Status_SUCCESS) {
            m_state = RUNNING; // change status of current master from REGISTERING to RUNNING
            LOG(INFO) << self() << " registered from super_master " << super_master << " successfully";
            if (!is_passive) {
                OwnedSlavesMessage *owned_slaves = new OwnedSlavesMessage();
                for (const string &slave_ip:m_alive_slaves) {
                    SlaveInfo *t_slave = owned_slaves->add_slave_infos();
                    HardwareResourcesMessage *hardware_resources = new HardwareResourcesMessage(
                            m_proto_hardware_resources[slave_ip]);
                    t_slave->set_allocated_hardware_resources(hardware_resources);
                    RuntimeResourcesMessage *runtime_Resources = new RuntimeResourcesMessage(
                            m_proto_runtime_resources[slave_ip]);
                    t_slave->set_allocated_runtime_resources(runtime_Resources);
                }
                owned_slaves->set_quantity(owned_slaves->slave_infos_size());
                send(super_master, *owned_slaves);
                delete owned_slaves;
                LOG(INFO) << " send owned slaves of " << self() << " to super_master " << super_master;
            }


        } else {
            LOG(INFO) << self() << "cannot registered to " << super_master
                      << ". Maybe it has registered to other supermaster before";

        }
    }

    void
    Master::received_terminating_master_message(const UPID &super_master, const TerminatingMasterMessage &message) {
        LOG(INFO) << " receive a TerminatingMasterMessage from " << super_master;
        if (message.master_id() == stringify(self().address.ip)) {
            LOG(INFO) << self() << "  is terminating due to new super_master was deteched";
            terminate(self());
        } else{
            ReregisterMasterMessage *reregister_master_message = new ReregisterMasterMessage();
            reregister_master_message->set_master_ip(message.master_id());
            reregister_master_message->set_port("6061");
//            MasterRegisteredMessage *master_registered_message = new MasterRegisteredMessage();
//            master_registered_message->set_master_id(stringify(message.master_id()));
//            master_registered_message->set_master_uuid(m_uuid);
//            master_registered_message->set_status(MasterRegisteredMessage_Status_FIRST_REGISTERING);
            for (auto iter = m_alive_slaves.begin(); iter != m_alive_slaves.end(); iter++) {
                LOG(INFO) << *iter;
                reregister_master_message->set_slave_ip(*iter);
                UPID slave_id("slave@"+*iter+":6061");
                send(slave_id,*reregister_master_message);
                LOG(INFO) << self().address.ip << " send new_master_message to salve: " <<slave_id;
            }
            delete reregister_master_message;
            terminate(self());
        }

    }

    void Master::get_select_master(const UPID& from, const string& message) {
        LOG(INFO) << "MAKUN received select_master_message";
        send(from,"MAKUN2");
    }
    // end of super_mater related

    std::ostream &operator<<(std::ostream &stream, const mesos::TaskState &state) {
        return stream << TaskState_Name(state);
    }
}

using namespace chameleon;


int main(int argc, char **argv) {
    chameleon::set_storage_paths_of_glog("master");// provides the program name
    chameleon::set_flags_of_glog();

    google::SetUsageMessage("usage : Option[name] \n"
                            "--port     the port used by the program");
    google::SetVersionString("Chameleon v1.0");
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::CommandLineFlagInfo info;

    if (GetCommandLineFlagInfo("port", &info) && info.is_default) {
        LOG(INFO) << "To run this program , must set parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    } else {
        if (GetCommandLineFlagInfo("port", &info) && !info.is_default) {
            os::setenv("LIBPROCESS_PORT", stringify(FLAGS_port));
            process::initialize("master");

            Master master;
            PID<Master> cur_master = process::spawn(master);
            LOG(INFO) << "Running master on " << process::address().ip << ":" << process::address().port;

            const PID<Master> master_pid = master.self();
            LOG(INFO) << master_pid;
            process::wait(master.self());
        } else {
            LOG(INFO) << "To run this program , must set all parameters correctly "
                         "\n read the notice " << google::ProgramUsage();
        }
    }
    return 0;
}

