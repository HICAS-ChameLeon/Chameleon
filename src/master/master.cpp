/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */

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
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        install<ParticipantInfo>(&Master::register_participant, &ParticipantInfo::hostname);

        install<HardwareResourcesMessage>(&Master::update_hardware_resources);
        install<JobMessage>(&Master::job_submited);
        install<RuntimeResourcesMessage>(&Master::received_heartbeat);

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

//        install<ReplyShutdownMessage>(&Master::received_reply_shutdown_message,&ReplyShutdownMessage::slave_ip, &ReplyShutdownMessage::is_shutdown);

        /**
         * Function  :  install schedule
         * Author    :  weiguow
         * Date      :  2018-12-27
         * */
        install<mesos::scheduler::Call>(&Master::receive);

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

        process::dispatch(self(), &Master::Offer, from);
        return;
    }

    /**
     * Function model  :  sprak run on chameleon
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
        mem_scalar->set_value(1500.0);
        mem_resource->mutable_scalar()->CopyFrom(*mem_scalar);
        offer->add_resources()->MergeFrom(*mem_resource);

        mesos::OfferID offerId;
        offerId.set_value("33333333");
        offer->mutable_id()->CopyFrom(offerId);

        offer->mutable_framework_id()->MergeFrom(m_frameworkID);

        mesos::SlaveID *slaveID = new mesos::SlaveID();
        slaveID->set_value("44444444");

        offer->mutable_slave_id()->MergeFrom(*slaveID);

        offer->set_hostname(self().address.hostname().get());

        mesos::internal::ResourceOffersMessage message;
        message.add_offers()->MergeFrom(*offer);
        message.add_pids("55555555");

        LOG(INFO) << "Sending " << message.offers().size() << " offer to slave";

        send(from, message);

        return;
    }

    /**
    * Function model  :  sprak run on chameleon
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
                    LOG(INFO) << "Get offer from scheduler";
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

                        mesos::TaskInfo task_(task);

                        LOG(INFO) << "Send task to slave ";
                        mesos::internal::RunTaskMessage message;
                        message.mutable_framework()->MergeFrom(m_frameworkInfo);
                        message.set_pid(from);
                        message.mutable_task()->MergeFrom(task_);
//                        message.set_allocated_framework_id(*m_frameworkId);
                        message.mutable_framework_id()->MergeFrom(m_frameworkID);

                        //SlavePID : slave(1)@172.20.110.152:5051
                        for (auto it = this->m_alive_slaves.begin(); it != this->m_alive_slaves.end(); it++) {
                            m_slavePID = "slave@" + stringify(*it) + ":6061";
                            send(m_slavePID, message);
                        }
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
        LOG(INFO) << "Status update " << update.status().state() << "(UUID: "
                  <<  update.mutable_uuid() << ")"
                  << " for task 0 of framework " << update.framework_id().value()
                  << " from agent " << update.slave_id().value();

        if (update.has_uuid()) {
            update.mutable_status()->set_uuid(update.uuid());
        }

        if (update.has_framework_id()) {
            LOG(INFO) << "Forwarding status update " << update.status().state() << "(UUID: "
                      << update.uuid() << ")"
                      << " for task 0 of framework " << update.framework_id().value();

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
     * Description  : get statusUpdateAcknowledge message from ??? and send it to slave
     * */
    void Master::statusUpdateAcknowledgement(
            const UPID& from,
            const mesos::SlaveID& slaveId,
            const mesos::FrameworkID& frameworkId,
            const mesos::TaskID& taskId,
            const string& uuid)
    {
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
    void Master::acknowledge(const mesos::scheduler::Call::Acknowledge& acknowledge)
    {
        const mesos::SlaveID& slaveId = acknowledge.slave_id();
        const mesos::TaskID& taskId = acknowledge.task_id();
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
            m_alive_slaves.insert(slaveid);
        }
    }

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

