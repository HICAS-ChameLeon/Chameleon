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
//              handle_accept_call(call.accept());
                break;


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

        this->frameworkInfo = frameworkInfo;

        /**
         * WEIGUO FRAMEWORK FROM: scheduler-22b6d4f4-7003-4956-95f9-eaf955c2ba55@172.20.110.114:44151
         * WEIGUO FRAMEWORK ROLE: *
         * WEIGUO FRAMEWORK PRINCAL:
         * WEIGUO FRAMEWORK SPACEUSED:  310
         * WEIGUO FRAMEWORK USER: weiguow
         * */
//        LOG(INFO) << "WEIGUO FRAMEWORK FROM: " << from;
//        LOG(INFO) << "WEIGUO FRAMEWORK ROLE: " << frameworkInfo.role();
//        LOG(INFO) << "WEIGUO FRAMEWORK PRINCAL: " << frameworkInfo.principal();
//        LOG(INFO) << "WEIGUO FRAMEWORK SPACEUSED:  " << frameworkInfo.SpaceUsed();
//        LOG(INFO) << "WEIGUO FRAMEWORK USER: " << frameworkInfo.user();

        //from == frameworkpid
        LOG(INFO) << "WEIGUO RECEIVED A SUBSCRIBE FRAMEWORK "
                  << frameworkInfo.name() << "  at " << from;


        //frameworkid WEIGUO NEWFRAMEWORKID:  79986764-d498-4d83-bfa2-8f4bd0da74f3-0000

        mesos::internal::FrameworkRegisteredMessage message;

        //Masterinfo,we should get this infomation from struct
        mesos::MasterInfo masterInfo;
        masterInfo.set_id("11111111");
        masterInfo.set_ip(self().address.ip.in().get().s_addr);
        masterInfo.set_port(6060);

        //frameworkID
        std::ostringstream out;
        int64_t nextFrameworkId;
        mesos::FrameworkID* frameworkID = new mesos::FrameworkID();
        out << masterInfo.id() << "-" << std::setw(4)
            << std::setfill('0') << nextFrameworkId++;
        frameworkID->set_value(out.str());
        this->frameworkID = frameworkID;

        message.mutable_framework_id()->MergeFrom(*frameworkID);
        message.mutable_master_info()->MergeFrom(masterInfo);

        send(from, message);

        LOG(INFO) << "WEIGUO SUBCRIBING FRAMEWORK " << frameworkInfo.name() << " SUCCESSFULL !";

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
        LOG(INFO) << "WEIGUO Resource_offer";
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

        offer->mutable_framework_id()->MergeFrom(*this->frameworkID);

        mesos::SlaveID *slaveID = new mesos::SlaveID();
        slaveID->set_value("44444444");

        offer->mutable_slave_id()->MergeFrom(*slaveID);

        offer->set_hostname(self().address.hostname().get());

        mesos::internal::ResourceOffersMessage message;
        message.add_offers()->MergeFrom(*offer);
        message.add_pids("55555555");

        LOG(INFO) << "WEIGUO SENDING " << message.offers().size() << " OFFER TO SLAVE";

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
                    LOG(INFO) << "WEIGUO GET OFFER FROM SCHEDULER";
                } else {
                    LOG(INFO) << "WEIGUO THERE IS NO TASK";
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

        mesos::Resources offeredResources;
        mesos::SlaveID slaveID;

        //for a single slave
//        if (accept.offer_ids().size() == 0) {
//            LOG(INFO) << "No offers specified";
//        } else {
//            foreach (const mesos::OfferID &offerId, accept.offer_ids()) {
//                //get offer resource
//                mesos::Offer *offer = getOffer(offerId);
//                if (offer != nullptr) {
//                    {
//                        /*
//                         * @param slaveID    :  for send to _accept
//                         * @offeredResources :  for send to _accept
//                         * */
//                        slaveID = offer->slave_id();
//                        offeredResources += offer->resources();
//                    }
//                    //actually,it's not right to delete offer
//                    delete (offer);
//                    continue;
//                }
//                // If the offer was not in our offer set, then this offer is no
//                LOG(WARNING) << "WEIGUO there is no resource in offer";
//            }
//        }

        vector<mesos::Offer::Operation> operations;

        foreach (const mesos::Offer::Operation &operation, accept.operations()) {
            switch (operation.type()) {
                case mesos::Offer::Operation::LAUNCH: {
                    mesos::Offer::Operation _operation;

                    _operation.set_type(mesos::Offer::Operation::LAUNCH);

                    foreach (const mesos::TaskInfo &task, operation.launch().task_infos()) {

                        mesos::TaskInfo task_(task);
                        LOG(INFO) << "WEIGUO SEND TASK TO SLAVE";

                        mesos::internal::RunTaskMessage message;


                        //FrameworkInfo
                        message.mutable_framework()->MergeFrom(this->frameworkInfo);

                        //FrameworkPid scheduler-6db9a175-a12d-4c96-85a1-2afd9561f0e2@172.20.110.152:37983
                        LOG(INFO) << "WEIGUO SEND TO SLAVE FROM" << from;
                        message.set_pid(from);

                        //TaskInfo
                        LOG(INFO) << "WEIGUO SEND TO SLAVE TASK" << task_.data();

                        message.mutable_task()->MergeFrom(task_);

                        message.set_allocated_framework_id(this->frameworkID);

                        //SlavePID : slave(1)@172.20.110.152:5051
                        for(auto it=this->m_alive_slaves.begin();it!=this->m_alive_slaves.end();it++){
                            string slavePID = "slave@"+stringify(*it)+":6061";
                            send(slavePID, message);
                        }
//                        string slavePID = "slave@172.20.110.228:6061";
//                        send(slavePID, message);

                        _operation.mutable_launch()->add_task_infos()->CopyFrom(task);
                        break;
                    }
                }
                case mesos::Offer::Operation::UNKNOWN: {
                    LOG(WARNING) << "Ignoring unknown offer operation";
                    break;
                }

            }
        }
    }


    void Master::handle_accept_call(mesos::scheduler::Call::Accept accept) {
        int   offers_size= accept.offer_ids_size();
        LOG(INFO)<<"lele handle_accept_call offers_size ="<<offers_size;
        int operations_size = accept.operations_size();
        LOG(INFO)<<" lele operations_size="<<operations_size;
        for(mesos::Offer_Operation offer_operation:accept.operations()){
            LOG(INFO)<<"lele offer_operation.type "<<offer_operation.type();
            if(offer_operation.type() == mesos::Offer_Operation::LAUNCH){
                mesos::Offer_Operation_Launch launch_message = offer_operation.launch();
                int current_tasks_size = launch_message.task_infos_size();
                LOG(INFO)<<"lele launch_message "<<current_tasks_size;
                for(int i=0;i<current_tasks_size;i++){
                    LOG(INFO)<<"lele at "<<i<< " task";
                    mesos::TaskInfo current_task = launch_message.task_infos(i);
                    LOG(INFO)<<" task name: "<<current_task.name();
                    LOG(INFO)<<" task_id: "<<current_task.task_id().value();
                    LOG(INFO)<<" slave_id: "<<current_task.slave_id().value();
                    if(current_task.has_command()){
                        mesos::CommandInfo command_info = current_task.command();
                        LOG(INFO)<<" command value "<<command_info.value();
                        int env_variables_size = command_info.environment().variables_size();
                        LOG(INFO)<< " command environment.variables.size() "<<env_variables_size;
                        for(int i=0;i<env_variables_size;i++){
                            mesos::Environment_Variable env_variable = command_info.environment().variables(i);
                            LOG(INFO)<<"environment variable "<<i<<" name is: "<<env_variable.name();
                            if(env_variable.type() == mesos::Environment_Variable_Type_VALUE){
                                LOG(INFO)<<"environment variable "<<i<<"'s type is VALUE";
                                LOG(INFO)<<"environment variable "<<i<<" value is : "<<env_variable.value();
                            }else if(env_variable.type() == mesos::Environment_Variable_Type_SECRET){
                                LOG(INFO)<<"environment variable "<<i<<"'s type is SECRET";
                                LOG(INFO)<<"environment variable "<<i<<" secret is : "<<env_variable.secret().SerializeAsString();
                            }
                        }
                        int uris_size = command_info.uris_size();
                        LOG(INFO)<<" command commandInfo_uris.size() "<<uris_size;
                        for(int i=0;i<uris_size;i++){
                            mesos::CommandInfo_URI commandInfo_uri = command_info.uris(i);
                            LOG(INFO)<<" commandInfo_uri.value "<<commandInfo_uri.value();
                            LOG(INFO)<<" commandInfo_uri.executable "<<commandInfo_uri.executable();
                            LOG(INFO)<<" commandInfo_uri.extract "<<commandInfo_uri.extract();
                            LOG(INFO)<<" commandInfo_uri.cache "<<commandInfo_uri.cache();
                            LOG(INFO)<<" commandInfo_uri.output_file "<<commandInfo_uri.output_file();
                        }
                    }
                }
            }
        }
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

    void
    Master::received_heartbeat(const UPID &slave, const RuntimeResourcesMessage &runtime_resouces_message) {
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

