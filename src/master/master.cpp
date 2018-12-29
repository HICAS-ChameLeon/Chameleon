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
     * Function model  :  sprak run on chameleon
     * Author          :  weiguow
     * Date            :  2018-12-27
     * Funtion name    :  receive, subscribe
     * */
    Framework* chameleon::Master::getFramework(const mesos::FrameworkID& frameworkId) const
    {
        return frameworks.registered.contains(frameworkId)
               ? frameworks.registered.at(frameworkId)
               : nullptr;
    }
    void Master::receive(const UPID &from, const mesos::scheduler::Call &call) {

        //first call
        if (call.type() == mesos::scheduler::Call::SUBSCRIBE) {
            subscribe(from, call.subscribe());
            return;
        }

        Framework *framework = getFramework(call.framework_id());

        switch (call.type()) {
            case mesos::scheduler::Call::SUBSCRIBE:
                // SUBSCRIBE call should have been handled above.
                LOG(FATAL) << "Unexpected 'SUBSCRIBE' call";
                break;

            case mesos::scheduler::Call::ACCEPT:
                LOG(INFO) << "Accept resource offer";

                accept(framework, call.accept());

//                mesos::scheduler::Call *copy_call;
//                copy_call = new mesos::scheduler::Call();
//                copy_call->CopyFrom(call);
                break;

            case mesos::scheduler::Call::UNKNOWN:
                LOG(WARNING) << "'UNKNOWN' call";
                break;
        }
    }

    void Master::subscribe(const UPID &from, const mesos::scheduler::Call::Subscribe &subscribe) {
        mesos::FrameworkInfo frameworkInfo = subscribe.framework_info();
        LOG(INFO) << "Weiguo Received SUBSCRIBE call for"
                  << " framework '" << frameworkInfo.name() << "' at " << from;

        mesos::internal::FrameworkRegisteredMessage message;
        mesos::MasterInfo masterInfo;

        masterInfo.set_ip(self().address.ip.in().get().s_addr);
        masterInfo.set_port(6060);
        masterInfo.set_id("11111111");

        // First subscribe
        mesos::FrameworkID *frameworkId = new mesos::FrameworkID();
        frameworkId->set_value("22222222");
        message.mutable_framework_id()->MergeFrom(*frameworkId);

        message.mutable_master_info()->MergeFrom(masterInfo);
        send(from, message);

        LOG(INFO) << "Weiguo Subscribing framework " << frameworkInfo.name()
                  << "Successful";

        process::dispatch(self(), &Master::dispatch_offer, from);
        return;
    }

    /**
     * Function model  :  sprak run on chameleon
     * Author          :  weiguow
     * Date            :  2018-12-28
     * Funtion name    :  Master::offer
     * */
    void Master::dispatch_offer(const UPID &from) {
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

        mesos::FrameworkID frameworkId;
        frameworkId.set_value("22222222");
        offer->mutable_framework_id()->MergeFrom(frameworkId);

        mesos::SlaveID *slaveID = new mesos::SlaveID();
        slaveID->set_value("44444444");

        offer->mutable_slave_id()->MergeFrom(*slaveID);

        offer->set_hostname("weiguow");


        mesos::internal::ResourceOffersMessage message;
        message.add_offers()->MergeFrom(*offer);
        message.add_pids("55555555");

        LOG(INFO) << "Sending " << message.offers().size();

        send(from, message);

        return;
    }

    /**
    * Function model  :  sprak run on chameleon
    * Author          :  weiguow
    * Date            :  2018-12-29
    * Funtion name    :  Master::accept
    * */

    mesos::Offer *Master::getOffer(const mesos::OfferID &offerId) const {
        return offers.contains(offerId) ? offers.at(offerId) : nullptr;
    }

    void chameleon::Master::accept(const Framework& framework, mesos::scheduler::Call::Accept accept) {

        mesos::FrameworkID frameworkId;
        frameworkId.set_value("22222222");

        mesos::OfferID offerId;
        offerId.set_value("33333333");

        mesos::SlaveID *slaveID = new mesos::SlaveID();
        slaveID->set_value("44444444");

        for (int i = 0; i < accept.operations_size(); ++i) {
            mesos::Offer::Operation *operation = accept.mutable_operations(i);

            if (operation->type() == mesos::Offer::Operation::LAUNCH) {
                if (operation->launch().task_infos().size() > 0) {
                    ++metrics->messages_launch_tasks;
                } else {
                    ++metrics->messages_decline_offers;
                    LOG(WARNING) << " in ACCEPT call for framework " << frameworkId
                                 << " as the launch operation specified no tasks";
                }
            } else if (operation->type() == mesos::Offer::Operation::LAUNCH_GROUP) {
                const mesos::ExecutorInfo &executor = operation->launch_group().executor();

                mesos::TaskGroupInfo *taskGroup = operation->mutable_launch_group()->mutable_task_group();

                // Mutate `TaskInfo` to include `ExecutorInfo` to make it easy
                // for operator API and WebUI to get access to the corresponding
                // executor for tasks in the task group.
                for (int j = 0; j < taskGroup->tasks().size(); ++j) {
                    mesos::TaskInfo *task = taskGroup->mutable_tasks(j);
                    if (!task->has_executor()) {
                        task->mutable_executor()->CopyFrom(executor);
                    }
                }
            }
        }

        mesos::Resources offeredResources;
        Option<mesos::Resource::AllocationInfo> allocationInfo = None();

        //for a single slave
        if (accept.offer_ids().size() == 0) {
            LOG(INFO) << "No offers specified";
        } else {
            // Compute offered resources and remove the offers. If the
            // validation failed, return resources to the allocator.
            foreach (const mesos::OfferID &offerId, accept.offer_ids()) {
                mesos::Offer *offer = getOffer(offerId);
                if (offer != nullptr) {
                    allocationInfo = offer->allocation_info();
                    offeredResources += offer->resources();
                }
                delete offer;
                continue;
            }
            // If the offer was not in our offer set, then this offer is no
            // longer valid.
            LOG(WARNING) << "Ignoring accept of offer " << offerId
                         << " since it is no longer valid";
        }


        mesos::Resources _offeredResources = offeredResources;
        mesos::Resources offeredSharedResources = offeredResources.shared();
        vector<mesos::Offer::Operation> operations;

        foreach (const mesos::Offer::Operation &operation, accept.operations()) {
            switch (operation.type()) {
                case mesos::Offer::Operation::LAUNCH: {
                    // For the LAUNCH operation we drop invalid tasks. Therefore
                    // we create a new copy with only the valid tasks to pass to
                    // the allocator.
                    mesos::Offer::Operation _operation;
                    _operation.set_type(mesos::Offer::Operation::LAUNCH);

                    foreach (const mesos::TaskInfo &task, operation.launch().task_infos()) {

                        mesos::TaskInfo task_(task);

                        // We add back offered shared resources for validation even if they
                        // are already consumed by other tasks in the same ACCEPT call. This
                        // allows these tasks to use more copies of the same shared resource
                        // than those being offered. e.g., 2 tasks can be launched on 1 copy
                        // of a shared persistent volume from the offer; 3 tasks can be
                        // launched on 2 copies of a shared persistent volume from 2 offers.
                        mesos::Resources available =
                                _offeredResources.nonShared() + offeredSharedResources;


                        // Add task.
//                        const mesos::Resources consumed = addTask(task_, framework, slave);

//                        CHECK(available.contains(consumed))
//                        << available << " does not contain " << consumed;

//                        _offeredResources -= consumed;

                        // TODO(bmahler): Consider updating this log message to
                        // indicate when the executor is also being launched.
//                        LOG(INFO) << "Launching task " << mesos::task_.task_id()
//                                  << " of framework " << framework
//                                  << " with resources " << task_.resources()
//                                  << " on agent " << *slave;

                        mesos::internal::RunTaskMessage message;
                        message.mutable_framework()->MergeFrom(framework.info);

                        // TODO(anand): We set 'pid' to UPID() for http frameworks
                        // as 'pid' was made optional in 0.24.0. In 0.25.0, we
                        // no longer have to set pid here for http frameworks.
                        message.set_pid(framework.pid.getOrElse(UPID()));
                        message.mutable_task()->MergeFrom(task_);

//                        if (HookManager::hooksAvailable()) {
//                            // Set labels retrieved from label-decorator hooks.
//                            message.mutable_task()->mutable_labels()->CopyFrom(
//                                    HookManager::masterLaunchTaskLabelDecorator(
//                                            task_,
//                                            framework.info,
//                                            slave->info));
//                        }

                        send(slave->pid, message);


                        _operation.mutable_launch()->add_task_infos()->CopyFrom(task);
                    }

                    operations.push_back(_operation);

                    break;
                }

                case mesos::Offer::Operation::LAUNCH_GROUP: {
                    // We must ensure that the entire group can be launched. This
                    // means all tasks in the group must be authorized and valid.
                    // If any tasks in the group have been killed in the interim
                    // we must kill the entire group.
                    const mesos::ExecutorInfo &executor = operation.launch_group().executor();
                    const mesos::TaskGroupInfo &taskGroup = operation.launch_group().task_group();


                    // Now launch the task group!
                    mesos::internal::RunTaskGroupMessage message;
                    message.mutable_framework()->CopyFrom(framework->info);
                    message.mutable_executor()->CopyFrom(executor);
                    message.mutable_task_group()->CopyFrom(taskGroup);

                    set<TaskID> taskIds;
                    Resources totalResources;

                    for (int i = 0; i < message.task_group().tasks().size(); ++i) {
                        TaskInfo *task = message.mutable_task_group()->mutable_tasks(i);

                        taskIds.insert(task->task_id());
                        totalResources += task->resources();

                        const Resources consumed = addTask(*task, framework, slave);

                        CHECK(_offeredResources.contains(consumed))
                        << _offeredResources << " does not contain " << consumed;

                        _offeredResources -= consumed;

                        if (HookManager::hooksAvailable()) {
                            // Set labels retrieved from label-decorator hooks.
                            task->mutable_labels()->CopyFrom(
                                    HookManager::masterLaunchTaskLabelDecorator(
                                            *task,
                                            framework->info,
                                            slave->info));
                        }
                    }

                    LOG(INFO) << "Launching task group " << stringify(taskIds)
                              << " of framework " << *framework
                              << " with resources " << totalResources
                              << " on agent " << *slave;

                    send(slave->pid, message);
                    break;
                }

                case Offer::Operation::UNKNOWN: {
                    LOG(WARNING) << "Ignoring unknown offer operation";
                    break;
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

