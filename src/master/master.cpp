/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */

#include <slave_related.pb.h>
#include "master.hpp"

//The following has default value
DEFINE_int32(port, 6060, "master run on this port");
DEFINE_string(supermaster_path, "/home/marcie/chameleon/Chameleon1/Chameleon/build/src/master/super_master",
              "the absolute path of supermaster executive. For example, --supermaster_path=/home/lemaker/open-source/Chameleon/build/src/master/super_master");
DEFINE_string(webui_path, "",
              "the absolute path of webui. For example, --webui=/home/lemaker/open-source/Chameleon/src/webui");
DEFINE_bool(fault_tolerance, false,"whether master has fault tolerance. For example, --fault_tolerance=true");


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

/**
 * Function name  : ValidateStr
 * Author         : weiguow
 * Date           : 2018-12-13
 * Description    : Determines whether the input parameter is valid
 * Return         : True or False*/
static bool ValidateStr(const char *flagname, const string &value) {
    if (!value.empty()) {
        return true;
    }
    printf("Invalid value for --%s: To run this program, you must set a meaningful value for it "
           "%s\n", flagname, value.c_str());;
    return false;
}

static bool validate_super_master_path(const char *flagname, const string &value) {

    if (value.empty() || os::exists(value)) {
        return true;
    }
    printf("Invalid value for super_master_path, please make sure the super_master_path actually exist!\n");
    return false;

}

static bool validate_webui_path(const char *flagname, const string &value) {

    if (os::exists(value)) {
        return true;
    }
    printf("Invalid value for webui_path, please make sure the webui_path actually exist!");
    return false;

}

static const bool has_port_Int = gflags::RegisterFlagValidator(&FLAGS_port, &ValidateInt);
static const bool has_super_master_path = gflags::RegisterFlagValidator(&FLAGS_supermaster_path,
                                                                        &validate_super_master_path);
static const bool has_webui_path = gflags::RegisterFlagValidator(&FLAGS_webui_path, &validate_webui_path);

namespace chameleon {

    void Master::initialize() {

        m_uuid = UUID::random().toString();

        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        nextFrameworkId = 0;
        m_scheduler = make_shared<CoarseGrainedScheduler>();

        install<HardwareResourcesMessage>(&Master::update_hardware_resources);
        install<RuntimeResourcesMessage>(&Master::received_heartbeat);
        install<AcceptRegisteredMessage>(&Master::received_registered_message_from_super_master);

        //  send the status update message of the tasks from master to the specific framework
        install<mesos::internal::StatusUpdateMessage>(
                &Master::status_update,
                &mesos::internal::StatusUpdateMessage::update,
                &mesos::internal::StatusUpdateMessage::pid);

        // send the status update acknowledement message from master to the specific slave
        install<mesos::internal::StatusUpdateAcknowledgementMessage>(
                &Master::status_update_acknowledgement,
                &mesos::internal::StatusUpdateAcknowledgementMessage::slave_id,
                &mesos::internal::StatusUpdateAcknowledgementMessage::framework_id,
                &mesos::internal::StatusUpdateAcknowledgementMessage::task_id,
                &mesos::internal::StatusUpdateAcknowledgementMessage::uuid);

        install<LaunchMasterMessage>(&Master::launch_master);
        install<SuperMasterControlMessage>(&Master::super_master_control);
        install<TerminatingMasterMessage>(&Master::received_terminating_master_message);

        install<BackupMasterMessage>(&Master::received_launch_backup_master);

//        install<ReplyShutdownMessage>(&Master::received_reply_shutdown_message,&ReplyShutdownMessage::slave_ip, &ReplyShutdownMessage::is_shutdown);

        /**
         * Function  :  install schedule
         * Author    :  weiguow
         * Date      :  2018-12-27
         * */
        install<mesos::scheduler::Call>(&Master::receive);

//        install<TerminatingMasterMessage>

        route(
                "/get-scheduler",
                "get the information of scheduler",
                [this](Request request) {
                    JSON::Object goarse_schedular ;
                    JSON::Object smhc_schedular ;
                    JSON::Object a_content = JSON::Object();
                    JSON::Array schedular_array;
                    const string &scheduler_name = m_scheduler->m_scheduler_name;

                    goarse_schedular.values["name"] = scheduler_name;
                    goarse_schedular.values["id"] = "1";
                    goarse_schedular.values["done"]= true;

                    smhc_schedular.values["name"] = "SMHCGrained";
                    smhc_schedular.values["id"] = "2";
                    smhc_schedular.values["done"]= false;


                    schedular_array.values.emplace_back(goarse_schedular);
                    schedular_array.values.emplace_back(smhc_schedular);

                    a_content.values["content"] = schedular_array;


                    OK ok_response(stringify(a_content));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
                });


        route(
                "/change-scheduler",
                "post a file",
                [this](Request request) {
                    string request_method = request.method;
                    LOG(INFO)<<"Starting get "<< request_method <<" request from Client";

                    string& tpath = request.url.path;
                    LOG(INFO)<<request.url;

                    string body_str = request.body;
                    LOG(INFO)<<body_str;
                    vector<string> str_scheduler = strings::split(body_str, "=");
                    string str_scheduler_name = str_scheduler[1];
                    LOG(INFO) << "The select scheduler is " << str_scheduler_name;

                    if(str_scheduler_name=="SMHCGrained"){
                        m_scheduler = make_shared<SMHCGrainedScheduler>();
                    }else{ // CoarseGrained
                        m_scheduler = make_shared<CoarseGrainedScheduler>();
                    }
                    const string &scheduler_name = m_scheduler->m_scheduler_name;
                    LOG(INFO)<< scheduler_name;
//                    mesos::internal::ResourceOffersMessage message;
//                    auto it = this->frameworks.registered.begin();
//                    Framework *framework = it->second;
//                    const mesos::FrameworkID frameworkId = framework->id();
//                    m_scheduler->construct_offers(message,frameworkId,m_slave_objects);
                  // if(body_str==m_scheduler->m_scheduler_name){}

                    std::ostringstream result;
                    result << "{ \"result\": " <<"\"" <<request_method+tpath <<"\"" << "}";
                    JSON::Value body = JSON::parse(result.str()).get();
                    return OK(body);
                });


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
                            array.values.emplace_back(it->second);
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
                    JSON::Object resources = JSON::Object();
                    if (!this->m_runtime_resources.empty()) {
                        JSON::Array array;
                        auto slave = m_slave_objects.begin();
                        for (auto it = this->m_runtime_resources.begin();
                             it != this->m_runtime_resources.end(); it++) {
                            resources.values["resources"] = it->second;
                            shared_ptr<SlaveObject> &slave_object = slave->second;
                            resources.values["slave_hostname"] = slave_object->m_hostname;
                            slave++;   //每有一个runtime_resource增加，则slave的顺序也跟着增加，这样才slave输出的hostname才不重复
                            array.values.emplace_back(resources);
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

        route(
                "/frameworks",
                "get all the information of frameworks related with the current master",
                [this](Request request) {
                    JSON::Object a_framework;
                    JSON::Object a_content = JSON::Object();
                    if (!this->frameworks.registered.empty()) {
                        JSON::Array frameworks_array;
                        for (auto it = this->frameworks.registered.begin();
                             it != this->frameworks.registered.end(); it++) {
                            Framework *framework = it->second;
                            a_framework = JSON::protobuf(framework->info);
                            if (framework->state == Framework::ACTIVE) {
                                a_framework.values["state"] = "ACTIVE";
                            }
                            if (framework->state == Framework::INACTIVE) {
                                a_framework.values["state"] = "INACTIVE";
                            }
                            if (framework->state == Framework::RECOVERED) {
                                a_framework.values["state"] = "RECOVERED";
                            }
                            if (framework->state == Framework::DISCONNECTED) {
                                a_framework.values["state"] = "DISCONNECTED";
                            }

                            const string framework_id = framework->id().value();

                            // find the relevant resources consumped on different slaves of the framework
                            JSON::Array slaves_array;
                            double sum_cpus = 0;
                            double  sum_mem = 0;
                            if (!framework_id.empty() && m_framework_to_slaves.count(framework_id)) {
                                unordered_set<string> &slaves_uuids = m_framework_to_slaves[framework_id];
                                for (auto it = slaves_uuids.begin(); it != slaves_uuids.end(); it++) {
                                    shared_ptr<SlaveObject> &slave_object = m_slave_objects[*it];
                                    const ResourcesOfFramework &resources_of_framework = slave_object->m_framework_resources[framework_id];
                                    JSON::Object resources_record = JSON::Object();
                                    resources_record.values["slave_uuid"] = *it;
                                    resources_record.values["slave_ip"] = slave_object->m_ip;
                                    resources_record.values["cpus"] = resources_of_framework.m_consumped_cpus;
                                    sum_cpus += resources_of_framework.m_consumped_cpus;
                                    resources_record.values["mem"] = resources_of_framework.m_consumped_mem;
                                    sum_mem += resources_of_framework.m_consumped_mem;
                                    slaves_array.values.emplace_back(resources_record);
                                }
                            }

                            a_framework.values["slaves"] = slaves_array;
                            a_framework.values["cpus"] = sum_cpus;
                            a_framework.values["mem"] = sum_mem;

                            frameworks_array.values.emplace_back(a_framework);
                        }


                        a_content.values["quantity"] = frameworks_array.values.size();
                        a_content.values["content"] = frameworks_array;
                    } else {

                        a_content.values["quantity"] = 0;
                        a_content.values["content"] = JSON::Object();
                    }

                    OK ok_response(stringify(a_content));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return ok_response;
                });

        route(
                "/framework_id",
                "get all the information of frameworks related with the current master",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    if (!this->m_framework_to_slaves.empty()) {
                        JSON::Array array;
                        for (auto it = this->m_framework_to_slaves.begin();
                             it != this->m_framework_to_slaves.end(); it++) {
                            unordered_set<string> slave_uuid = it->second;
                            for (auto its = slave_uuid.begin(); its != slave_uuid.end(); its++) {
                                array.values.push_back(*its);
                            }
                            result.values["quantity"] = array.values.size();
                            result.values["content"] = array;
                        }
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
                        const UPID current_slave(chameleon::construct_UPID_string("slave", ip, "6061"));
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

        route(
                "/start_supermaster",
                "start supermaster by subprocess",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    /**
                      * Function model  :  start a subprocess of super_master
                      * Author          :  Jessicallo
                      * Date            :  2019-2-27
                      * Funtion name    :  Try
                      * @param          :
                      * */
                    // for example, --master_path=/home/lemaker/open-source/Chameleon/build/src/master/master
                    const string launcher =
                            m_super_master_path + " --master_path=" + m_master_cwd + "/master" + " --webui_path=" +
                            m_webui_path + " --level=2";
//                            m_super_master_path + " --master_path=/home/lemaker/open-source/Chameleon/build/src/master/master" + " --webui_path=" +
//                            m_webui_path + " --level=2";
                    Try<Subprocess> super_master = subprocess(
                            launcher,
                            Subprocess::FD(STDIN_FILENO),
                            Subprocess::FD(STDOUT_FILENO),
                            Subprocess::FD(STDERR_FILENO)
                    );
                    result.values["start"] = "success";
                    OK response(stringify(result));
                    response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return response;
                });

        route(
                "/start_three_supermaster",
                "start supermaster by subprocess",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    /**
                      * Function model  :  start a subprocess of super_master
                      * Author          :  Jessicallo
                      * Date            :  2019-2-27
                      * Funtion name    :  Try
                      * @param          :
                      * */
                    // for example, --master_path=/home/lemaker/open-source/Chameleon/build/src/master/master
                    const string launcher =
                            m_super_master_path + " --master_path=" + get_cwd() + "/master" + " --webui_path=" +
                            m_webui_path + " --level=3";
                    Try<Subprocess> super_master = subprocess(
                            launcher,
                            Subprocess::FD(STDIN_FILENO),
                            Subprocess::FD(STDOUT_FILENO),
                            Subprocess::FD(STDERR_FILENO)
                    );
                    result.values["start"] = "success";
                    OK response(stringify(result));
                    response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    return response;
                });

        provide("", path::join(m_webui_path, "static/HTML/Control.html"));
        provide("static", path::join(m_webui_path, "/static"));

//     install("stop", &MyProcess::stop);
        install("stop", [=](const UPID &from, const string &body) {
            terminate(self());
        });

        // super_master related
        // when we have only one level (i.e. we have no super_master),
        // is_passive = true stands fro that the master is started initiatively.
        is_passive = false;

        m_state = RUNNING;

        heartbeat_check_slaves();

    }

    // get the absolute path of the directory where the master executable exists
    const string Master::get_cwd() const {
        return m_master_cwd;
    }

    void Master::set_webui_path(const string &path) {
        m_webui_path = path;
    }

    void Master::set_fault_tolerance(bool fault_tolerance) {
        m_is_fault_tolerance = fault_tolerance;
    }

    const string Master::get_web_ui() const {
        return m_webui_path;
    }

    /**
      * Function model  :  spark run on chameleon
      * Author          :  weiguow
      * Date            :  2018-12-27
      * Funtion name    :  receive
      * @param          : UPID& from ,Call& call
      * */
    void Master::receive(const UPID &from, const mesos::scheduler::Call &call) {
        LOG(INFO)<<call.subscribe().framework_info().name();
        //first call
        if (call.type() == mesos::scheduler::Call::SUBSCRIBE) {
            subscribe(from, call.subscribe());
            return;
        }

        Framework *framework = get_framework(call.framework_id());

        if (framework == nullptr) {
            LOG(INFO) << "Framework cannot be found";
            return;
        }

        if (framework->pid != from) {
            LOG(INFO) << "Call is not from registered framework";
            return;
        }

        switch (call.type()) {
            case mesos::scheduler::Call::SUBSCRIBE:
                LOG(FATAL) << "Unexpected 'SUBSCRIBE' call";
                break;

            case mesos::scheduler::Call::TEARDOWN:
                teardown(framework);
                break;

            case mesos::scheduler::Call::ACCEPT:
                LOG(INFO) << "accept message from framework";
                accept(framework, call.accept());
                break;

            case mesos::scheduler::Call::DECLINE:
                decline(framework, call.decline());
                break;

            case mesos::scheduler::Call::SHUTDOWN:
                shutdown(framework, call.shutdown());
                break;

            case mesos::scheduler::Call::ACKNOWLEDGE: {
                Try<UUID> uuid = UUID::fromBytes(call.acknowledge().uuid());
                if (uuid.isError()) {
                    LOG(INFO) << "Ignoring unknow uuid" << uuid.get();
                    return;
                }
                acknowledge(framework, call.acknowledge());
                break;
            }

            case mesos::scheduler::Call::UNKNOWN:
                LOG(WARNING) << "'UNKNOWN' call";
                break;
        }
    }

    /**
     * Function model  :  spark run on chameleon
     * Author          :  weiguow
     * Date            :  2018-12-28
     * Funtion name    :  subscribe
     * @param          : UPID &from ,Call::Subscribe &subscribe
     * */
    void Master::subscribe(const UPID &from, const mesos::scheduler::Call::Subscribe &subscribe) {

        mesos::FrameworkInfo frameworkInfo = subscribe.framework_info();

//        LOG(INFO) << "Received  SUBSCRIBE call for framework "
//                  << frameworkInfo.name() << " at " << from;

        if (!frameworkInfo.has_id() || frameworkInfo.id().value().empty()) {

            // If we are here the framework is subscribing for the first time.
            // Check if this framework is already subscribed (because it retries).
            foreachvalue (Framework *framework, frameworks.registered) {
                                    if (framework->pid == from) {
//                                        LOG(INFO) << "Framework " << *framework
//                                                  << " already subscribed, resending acknowledgement";
                                        mesos::internal::FrameworkRegisteredMessage message;
                                        message.mutable_framework_id()->MergeFrom(framework->id());
                                        message.mutable_master_info()->MergeFrom(framework->master->m_masterInfo);
                                        framework->send(message);
                                        return;
                                    }
                                }

            mesos::internal::FrameworkRegisteredMessage message;

            frameworkInfo.mutable_id()->CopyFrom(newFrameworkId());
            Framework *framework = new Framework(this, frameworkInfo, from);

            add_framework(framework);

            message.mutable_framework_id()->MergeFrom(framework->id());
            message.mutable_master_info()->MergeFrom(m_masterInfo);

            framework->send(message);

            LOG(INFO) << "Subscribe framework " << frameworkInfo.name() << " successful!";
//
//            const Duration temp_duration = Seconds(0);
//            //
//            process::delay(temp_duration, self(), &Master::Offer, framework->id());
// after subscribed, the framework can be given resource offers.
            offer(framework->id());

            return;
        }
    }


    /**
     * Function model  :  spark run on chameleon
     * Author          :  weiguow
     * Date            :  2018-12-28
     * Funtion name    :  Master::offer
     * */
    void Master::offer(const mesos::FrameworkID &frameworkId) {

        Framework *framework = CHECK_NOTNULL(frameworks.registered.at(frameworkId.value()));

        mesos::internal::ResourceOffersMessage message;
        LOG(INFO) << "start scheduling to provide offers";

        m_scheduler->construct_offers(message, frameworkId, m_slave_objects);
//
//        m_scheduler->construct_offers(message,frameworkId,m_slave_objects);

       // m_smhc_scheduler->construct_offers(message,frameworkId,m_slave_objects);


        if (message.offers_size() > 0) {
            framework->send(message);
            LOG(INFO) << "Sent " << message.offers_size() << " offer to framework "
                      << framework->pid.get();
        } else {
            LOG(INFO)
                    << "available offer size is 0, the master doesn't have sufficient resources for the framework's requirement.";
        }
        return;
    }

    /**
    * Function model  :  spark run on chameleon
    * Author          :  weiguow
    * Date            :  2018-12-29
    * Funtion name    :  Master::accept
    * */
    void Master::accept(Framework *framework, mesos::scheduler::Call::Accept accept) {
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
//        ResourcesOfFramework resources_of_framework;
//        if(m_slave_objects.)
        foreach (const mesos::Offer::Operation &operation, accept.operations()) {
            switch (operation.type()) {
                case mesos::Offer::Operation::LAUNCH: {
                    mesos::Offer::Operation _operation;

                    _operation.set_type(mesos::Offer::Operation::LAUNCH);

                    foreach (const mesos::TaskInfo &task, operation.launch().task_infos()) {

                        if (m_slave_objects.count(task.slave_id().value()) > 0) {
                            shared_ptr<SlaveObject> &current_slave = m_slave_objects.at(task.slave_id().value());

                            // the framework will be launched on the current_slave, so we will check whether the current_slave has the framework running on.
                            if (current_slave->m_framework_resources.count(framework->id().value()) == 0) {
                                // construct the ResourceOfFramework first
                                current_slave->m_framework_resources[framework->id().value()];
                                if (m_framework_to_slaves.count(framework->id().value()) == 0) {
                                    m_framework_to_slaves[framework->id().value()];
                                }
                                m_framework_to_slaves[framework->id().value()].insert(current_slave->m_uuid);
                            }
                            // get the reference of the ResourceOfFramework of the current framework
                            ResourcesOfFramework &resources_of_framework = current_slave->m_framework_resources[framework->id().value()];

                            // first, get the actual resource consumption of the task because we want to calculate the available
                            // resource of the specified slave
                            auto consumption = task.resources();
                            for (auto it = consumption.begin(); it != consumption.end(); it++) {
                                LOG(INFO) << it->name() << " " << it->scalar().value();
                                if (it->name() == "cpus") {
                                    if (current_slave->m_available_cpus > it->scalar().value()) {
                                        current_slave->m_available_cpus -= it->scalar().value();
                                        resources_of_framework.m_consumped_cpus += it->scalar().value();
                                    } else {
                                        LOG(INFO) << " the available cpu resources of the " << current_slave->m_upid_str
                                                  << "cannot satisfy the cpu resources requirements of the task "
                                                  << task.name() << " So we need to offer resources for it again";
                                        break;
                                    }
                                } else if (it->name() == "mem") {
                                    if (current_slave->m_available_mem > it->scalar().value()) {
                                        current_slave->m_available_mem -= it->scalar().value();
                                        resources_of_framework.m_consumped_mem += it->scalar().value();
                                    } else {
                                        LOG(INFO) << " the available memory resources of the "
                                                  << current_slave->m_upid_str
                                                  << "cannot satisfy the memory resources requirements of the task "
                                                  << task.name() << " So we need to offer resources for it again";
                                        break;
                                    }
                                } else {

                                }

                            }
                            mesos::TaskInfo task_(task);
                            const process::UPID slave_upid = current_slave->m_upid;
                            LOG(INFO) << "Sending task to slave " << slave_upid; //slave(1)@172.20.110.152:5051

                            mesos::internal::RunTaskMessage message;
                            message.mutable_framework()->MergeFrom(framework->info);
                            message.set_pid(framework->pid.getOrElse(UPID()));
                            message.mutable_task()->MergeFrom(task_);
                            message.mutable_framework_id()->MergeFrom(framework->id());

                            send(slave_upid, message);

//                            _operation.mutable_launch()->add_task_infos()->CopyFrom(task);
                        }
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

    void Master::teardown(Framework *framework) {
        CHECK_NOTNULL(framework);

        LOG(INFO) << "Processing TEARDOWN call for framework " << *framework;

        remove_framework(framework);
    }


    void Master::decline(Framework *framework, const mesos::scheduler::Call::Decline &decline) {
        CHECK_NOTNULL(framework);
        for (auto i = decline.offer_ids().begin(); i != decline.offer_ids().end(); i++) {
            if (decline.offer_ids().size() == m_scheduler->m_offers.size()) {
                process::dispatch(self(), &Master::offer, framework->id());
            }
            else {
                LOG(INFO) << "Offer "<< i->value() << " has been declined by framework "
                << framework->pid.get();
                m_scheduler->m_offers.erase(i->value());
            }
        }
    }


    void Master::shutdown(Framework *framework, const mesos::scheduler::Call::Shutdown &shutdown) {
        CHECK_NOTNULL(framework);

        const mesos::SlaveID &slaveID = shutdown.slave_id();

//        const
    }

    /**
     * Function     : status_update
     * Author       : weiguow
     * Date         : 2019-1-10
     * Description  : get statusUpdate message from slave and send it to framework
     * */
    void Master::status_update(mesos::internal::StatusUpdate update, const UPID &pid) {

        LOG(INFO) << "Status update " << update.status().state()
                  << " from agent " << update.slave_id().value();

        Framework *framework = get_framework(update.framework_id());

        if (update.has_uuid()) {
            update.mutable_status()->set_uuid(update.uuid());
        }

        if (update.has_framework_id()) {

//            LOG(INFO) << "Sending status update " << update.status().state()
//                      << " to framework " << update.framework_id().value();

            mesos::internal::StatusUpdateMessage message;
            message.mutable_update()->MergeFrom(update);
            message.set_pid(pid);   //this pid is slavePID

            framework->send(message);
        }
    }

    /**
     * Function     : status_update_acknowledgement
     * Author       : weiguow
     * Date         : 2019-1-10
     * Description  : get statusUpdateAcknowledge message from slave
     * */
    void Master::status_update_acknowledgement(
            const UPID &from,
            const mesos::SlaveID &slaveId,
            const mesos::FrameworkID &frameworkId,
            const mesos::TaskID &taskId,
            const string &uuid) {
        Framework *framework = get_framework(frameworkId);

        mesos::scheduler::Call::Acknowledge message;
        message.mutable_slave_id()->CopyFrom(slaveId);
        message.mutable_task_id()->CopyFrom(taskId);
        message.set_uuid(uuid);

        acknowledge(framework, message);
    }

    /**
    * Function     : acknowledge
    * Author       : weiguow
    * Date         : 2019-1-10
    * Description  : send StatusUpdateAcknowledgementMessage message to
    * slave make sure the status
    * */
    void Master::acknowledge(Framework *framework, const mesos::scheduler::Call::Acknowledge &acknowledge) {
        const mesos::SlaveID &slaveId = acknowledge.slave_id();
        const mesos::TaskID &taskId = acknowledge.task_id();
        const UUID uuid = UUID::fromBytes(acknowledge.uuid()).get();

        mesos::internal::StatusUpdateAcknowledgementMessage message;
        message.mutable_slave_id()->CopyFrom(slaveId);

        message.mutable_framework_id()->MergeFrom(framework->id());
        message.mutable_task_id()->CopyFrom(taskId);
        message.set_uuid(uuid.toBytes());

        LOG(INFO) << "Sending acknowledge to slave " <<  m_slave_objects.at(slaveId.value())->m_upid;

        send(m_slave_objects.at(slaveId.value())->m_upid, message);
    }

    /**
     * Function     : addFramework
     * Author       : weiguow
     * Date         : 2019-2-22
     * Description  : Save Frameworkinfo to master
     * */
    void Master::add_framework(Framework *framework) {

        frameworks.registered[framework->id().value()] = framework;

        if (framework->connected()) {
            if (framework->pid.isSome()) {
                link(framework->pid.get());
            }
        }
    }

    /**
     * use frameworkId to get Framework-weiguow-2019/2/24
     * */
    Framework *Master::get_framework(const mesos::FrameworkID &frameworkId) {
        return frameworks.registered.contains(frameworkId.value())
               ? frameworks.registered.at(frameworkId.value())
               : nullptr;
    }

    /**
     * remove framework-weiguow-2019/2/26*
     * */
    void Master::remove_framework(Framework *framework) {
        CHECK_NOTNULL(framework);

        LOG(INFO) << "Removing framework " << *framework;

        // restore the resources occupied by the framework in the specific slave
        const string framework_id = framework->id().value();
        LOG(INFO) << "Removing " << framework_id;
        if (m_framework_to_slaves.count(framework_id)) {
            unordered_set<string> &slave_uuids = m_framework_to_slaves.at(framework_id);
            LOG(INFO) << "Removing 752";

            if (!slave_uuids.empty()) {
                for (auto it = slave_uuids.begin(); it != slave_uuids.end(); it++) {
                    shared_ptr<SlaveObject> &slave = m_slave_objects[*it];
                    if (slave != nullptr) {
                        LOG(INFO) << "restore begins";
                        if (slave->restore_resource_of_framework(framework_id)) {
                            if (framework->active()) {
//                            CHECK(framework->active());

                                LOG(INFO) << "Deactive framework " << *framework;

                                framework->state = Framework::State::INACTIVE;
                            }
                            //send ShutdownFrameworkMessage to slave
                            mesos::internal::ShutdownFrameworkMessage message;
                            message.mutable_framework_id()->MergeFrom(framework->id());

                            send(slave->m_upid, message);

                        }
                    } else {
                        LOG(INFO) << "slave == nullptr";
                    }

                }
            } else {
                LOG(INFO) << "slave_uuids == empty";

            }


            m_framework_to_slaves.erase(framework_id);
        }

    }


    mesos::FrameworkID Master::newFrameworkId() {
        std::ostringstream out;
        out << m_masterInfo.id() << "-" << std::setw(4)
            << std::setfill('0') << nextFrameworkId++;

        LOG(INFO) << "m_masterInfo.id(): " << m_masterInfo.id();

        mesos::FrameworkID frameworkId;
        frameworkId.set_value(out.str());

        return frameworkId;
    }


    void Master::register_participant(const string &hostname) {
        DLOG(INFO) << "master receive register message from " << hostname;
    }

    void Master::update_hardware_resources(const UPID &from,
                                           const HardwareResourcesMessage &hardware_resources_message) {
        DLOG(INFO) << "Enter update hardware resources";

        auto slaveid = hardware_resources_message.slave_id();

//        slaves.registering.insert(from);

        if (m_hardware_resources.find(slaveid) == m_hardware_resources.end()) {
            JSON::Object object = JSON::protobuf(hardware_resources_message);
            m_hardware_resources.insert({slaveid, object});
            m_proto_hardware_resources.insert({slaveid, hardware_resources_message});
            m_alive_slaves.insert(slaveid);

//            UPID temp_upid(from);
            shared_ptr<SlaveObject> slave_object = make_shared<SlaveObject>(from, hardware_resources_message);
            m_slave_objects.insert({slave_object->m_uuid, slave_object});
        }
    }

    void Master::received_heartbeat(const UPID &slave, const RuntimeResourcesMessage &runtime_resouces_message) {
        if(m_slave_objects.count(runtime_resouces_message.slave_uuid())) {
            LOG(INFO) << "received a heartbeat message from " << slave;
            auto slave_id = runtime_resouces_message.slave_id();
            m_runtime_resources[slave_id] = JSON::protobuf(runtime_resouces_message);
            m_proto_runtime_resources[slave_id] = runtime_resouces_message;
            //add insert slave_id to send new master message to slave
            m_alive_slaves.insert(slave_id);
            m_slaves_last_time[slave_id] = time(0);
            if (m_is_fault_tolerance && slave_id != stringify(process::address().ip)) {
                LaunchMasterMessage *launch_master_message = new LaunchMasterMessage();
                launch_master_message->set_port("6060");
                launch_master_message->set_master_path(get_cwd() + "/master");
                launch_master_message->set_webui_path(m_webui_path);
                launch_master_message->set_is_fault_tolerance(true);
                send(slave, *launch_master_message);
                delete launch_master_message;
                LOG(INFO) << "send launch backup master message to " << slave;
                m_is_fault_tolerance = false;
            }
        }
    }

    void Master::heartbeat_check_slaves() {
        delete_slaves();
        process::delay(Seconds(10), self(), &Self::heartbeat_check_slaves);
    }

    void Master::delete_slaves() {
        for(auto iter = m_alive_slaves.begin(); iter != m_alive_slaves.end(); iter++) {
            if (m_slaves_last_time[*iter] != 0 && time(0) - m_slaves_last_time[*iter] > 15) {
                LOG(INFO)<<"slave run on "<<*iter<<" was killed!";
                m_hardware_resources.erase(*iter);
                m_proto_hardware_resources.erase(*iter);
                m_runtime_resources.erase(*iter);
                m_proto_runtime_resources.erase(*iter);
                m_alive_slaves.erase(*iter);
            }
        }
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

    void Master::set_super_master_path(const string &path) {
        m_super_master_path = path;
        LOG(INFO) << "The path of super_master executable is " << m_super_master_path;
    }

    void Master::launch_master(const UPID &super_master, const LaunchMasterMessage &message) {
        send(super_master,"successed");
    }

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
            // so in super_master_related.proto at line 30 repeated SlavesInfoControlledByMaster my_slaves=4
            // is not empty
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

        if(super_master_control_message.my_master().size()){
            LOG(INFO) << self().address << " received message from " << super_master;
            string launch_command = m_super_master_path + " --initiator=" + stringify(self().address)
                    + " --master_path=/home/marcie/chameleon/Chameleon1/build/src/master/master --webui_path="
                    + stringify(FLAGS_webui_path) + " --port=7001";
            Try<Subprocess> s = subprocess(
                    launch_command,
                    Subprocess::FD(STDIN_FILENO),
                    Subprocess::FD(STDOUT_FILENO),
                    Subprocess::FD(STDERR_FILENO));
            if (s.isError()) {
                LOG(ERROR) << "cannot launch super_master "<< self().address.ip << ":7001";
//                send(super_master,"error");
            }
            LOG(INFO) << self().address.ip << ":7001 launched super_master successfully.";
//            send(super_master,"successed");
//            for(int i = 0; i < super_master_control_message.my_master().size(); i++){
//            }
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
            } else{
                m_super_master = super_master;
                heartbeat_to_supermaster();
            }


        } else {
            LOG(INFO) << self() << "cannot registered to " << super_master
                      << ". Maybe it has registered to other supermaster before";

        }
    }

    void Master::heartbeat_to_supermaster(){
        if(!m_proto_hardware_resources.empty()&&!m_proto_runtime_resources.empty()) {
            for (auto iter = m_proto_hardware_resources.begin(); iter != m_proto_hardware_resources.end(); iter++) {
                send(m_super_master, iter->second);
            }
            for (auto iter = m_proto_runtime_resources.begin(); iter != m_proto_runtime_resources.end(); iter++) {
                send(m_super_master, iter->second);
                LOG(INFO) << "send message to " << m_super_master;
            }
        }
        process::delay(m_interval, self(), &Self::heartbeat_to_supermaster);
    }

    void
    Master::received_terminating_master_message(const UPID &super_master, const TerminatingMasterMessage &message) {
        LOG(INFO) << " receive a TerminatingMasterMessage from " << super_master;
        if (message.master_id() == stringify(self().address.ip)) {
            LOG(INFO) << self() << "  is terminating due to new super_master was deteched";
            terminate(self());
        } else {
            ReregisterMasterMessage *reregister_master_message = new ReregisterMasterMessage();
            reregister_master_message->set_master_ip(message.master_id());
            reregister_master_message->set_port("6060");
//            MasterRegisteredMessage *master_registered_message = new MasterRegisteredMessage();
//            master_registered_message->set_master_id(stringify(message.master_id()));
//            master_registered_message->set_master_uuid(m_uuid);
//            master_registered_message->set_status(MasterRegisteredMessage_Status_FIRST_REGISTERING);
            for (auto iter = m_alive_slaves.begin(); iter != m_alive_slaves.end(); iter++) {
//                LOG(INFO) << *iter;
                reregister_master_message->set_slave_ip(*iter);
                UPID slave_id("slave@" + *iter + ":6061");
                send(slave_id, *reregister_master_message);
                LOG(INFO) << self() << " send new_master_message: " << reregister_master_message->master_ip()
                          << " to salve: " << slave_id;
            }
            delete reregister_master_message;
            LOG(INFO) << self() << " is terminating due to change levels to one";
            terminate(self());
            process::wait(self());
        }
    }
    // end of super_mater related

    void Master::received_launch_backup_master(const UPID &slave, const BackupMasterMessage &message) {
        LOG(INFO)<<"received BackupMasterMessage from "<<slave;
        for (auto iter = m_alive_slaves.begin(); iter != m_alive_slaves.end(); iter++) {
            UPID slave_id("slave@" + *iter + ":6061");
            send(slave_id,message);
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

    if (has_port_Int && has_super_master_path && has_webui_path) {
        os::setenv("LIBPROCESS_PORT", stringify(FLAGS_port));

        process::initialize("master");
        Master master;
        if (FLAGS_supermaster_path.empty()) {
            master.set_super_master_path(master.get_cwd() + "/super_master");
        } else {
            master.set_super_master_path(FLAGS_supermaster_path);
        }

        // set the webui path for the master
        master.set_webui_path(FLAGS_webui_path);

        master.set_fault_tolerance(FLAGS_fault_tolerance);

        PID<Master> cur_master = process::spawn(master);


        LOG(INFO) << "Running master on " << process::address().ip << ":" << process::address().port;

        const PID<Master> master_pid = master.self();
        LOG(INFO) << master_pid;
        process::wait(master.self());
    } else {
        LOG(INFO) << "To run this program , must set all parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    }
    return 0;
}
