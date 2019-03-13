/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-11-26
 * Description：master
 */
#include "master.hpp"

using namespace chameleon::master;

//The following has default value
DEFINE_int32(port, 6060, "master run on this port");
DEFINE_string(supermaster_path, "./super_master",
              "the absolute path of supermaster executive. For example, "
              "-supermaster_path=/home/lemaker/open-source/Chameleon/build/src/master/super_master");


static bool ValidateInt(const char *flagname, gflags::int32 value) {
    if (value >= 0 && value < 32768) {
        return true;
    }
    printf("Invalid value for --%s: %d\n", flagname, (int) value);
    return false;
}

static bool ValidateStr(const char *flagname, const string &value) {
    if (!value.empty()) {
        return true;
    }
    printf("Invalid value for --%s: To run this program, you must set a meaningful value for it "
           "%s\n", flagname, value.c_str());;
    return false;
}

const bool has_port_Int = gflags::RegisterFlagValidator(&FLAGS_port, &ValidateInt);
const bool has_super_master_path = gflags::RegisterFlagValidator(&FLAGS_supermaster_path, &ValidateStr);

namespace chameleon {
    namespace master {

        void Master::initialize() {
            // Verify that the version of the library that we linked against is
            // compatible with the version of the headers we compiled against.
            GOOGLE_PROTOBUF_VERIFY_VERSION;

            install<ParticipantInfo>(&Master::register_participant, &ParticipantInfo::hostname);

            install<HardwareResourcesMessage>(&Master::update_hardware_resources);

            install<RuntimeResourcesMessage>(&Master::received_heartbeat);

            install<AcceptRegisteredMessage>(&Master::received_registered_message_from_super_master);

            install<mesos::internal::StatusUpdateMessage>(
                    &Master::status_update,
                    &mesos::internal::StatusUpdateMessage::update,
                    &mesos::internal::StatusUpdateMessage::pid);

            install<mesos::internal::StatusUpdateAcknowledgementMessage>(
                    &Master::status_update_acknowledgement,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::slave_id,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::framework_id,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::task_id,
                    &mesos::internal::StatusUpdateAcknowledgementMessage::uuid);

            install<mesos::scheduler::Call>(&Master::receive);

            install<SuperMasterControlMessage>(&Master::super_master_control);

            install<TerminatingMasterMessage>(&Master::received_terminating_master_message);

//        install<ReplyShutdownMessage>(&Master::received_reply_shutdown_message,&ReplyShutdownMessage::slave_ip, &ReplyShutdownMessage::is_shutdown);

            //change two levels to one related
            install("MAKUN", &Master::get_select_master);
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

            route(
                    "/frameworks",
                    "wangweiguo new version of frameworks",
                    [this](Request request) {
                        JSON::Object a_framework;
                        JSON::Object a_content = JSON::Object();
                        if (!this->frameworks.registered.empty()) {
                            JSON::Array frameworks_array;
                            for (auto it = this->frameworks.registered.begin();
                                 it != this->frameworks.registered.end(); it++) {
                                Framework *framework = it->second;
                                a_framework = JSON::protobuf(framework->m_info);
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
                        m_super_master_path.append(" --master_path=/home/wqn/Chameleon/build/src/master/master");
                        Try<Subprocess> super_master = subprocess(
                                m_super_master_path,
                                Subprocess::FD(STDIN_FILENO),
                                Subprocess::FD(STDOUT_FILENO),
                                Subprocess::FD(STDERR_FILENO)
                        );
                        OK response(stringify(result));
                        response.headers.insert({"Access-Control-Allow-Origin", "*"});
                        return response;
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

        void Master::receive(const UPID &from, const mesos::scheduler::Call &call) {
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

            if (framework->m_pid != from) {
                LOG(INFO) << "Call is not from registered framework";
                return;
            }

            switch (call.type()) {
                case mesos::scheduler::Call::SUBSCRIBE:
                    LOG(FATAL) << "Unexpected 'SUBSCRIBE' call";
                    break;

                case mesos::scheduler::Call::TEARDOWN:
                    teardown_framework(framework);
                    break;

                case mesos::scheduler::Call::ACCEPT:
                    LOG(INFO) << "Accept message from framework " << *framework;
                    accept(framework, call.accept());
                    break;

                case mesos::scheduler::Call::DECLINE:
                    decline_framework(framework, call.decline());
                    break;

                case mesos::scheduler::Call::SHUTDOWN:
                    shutdown_slave_executor(framework, call.shutdown());
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

        void Master::subscribe(const UPID &from, const mesos::scheduler::Call::Subscribe &subscribe) {

            mesos::FrameworkInfo frameworkInfo = subscribe.framework_info();
            Framework *framework = get_framework(frameworkInfo.id());

            LOG(INFO) << "Received  SUBSCRIBE call for framework "
                      << frameworkInfo.name() << " at " << from;

            if (!frameworkInfo.has_id() || frameworkInfo.id().value().empty()) {

                // If we are here the framework is subscribing for the first time.
                // Check if this framework is already subscribed (because it retries).
                foreachvalue (Framework *framework, frameworks.registered) {
                                        if (framework->m_pid == from) {
                                            LOG(INFO) << "Framework " << *framework
                                                      << " already subscribed, resending acknowledgement";
                                            mesos::internal::FrameworkRegisteredMessage message;
                                            message.mutable_framework_id()->MergeFrom(framework->id());
                                            message.mutable_master_info()->MergeFrom(framework->m_master->m_masterinfo);
                                            framework->send(message);
                                            return;
                                        }
                                    }

                mesos::internal::FrameworkRegisteredMessage message;

                frameworkInfo.mutable_id()->CopyFrom(new_framework_id());
                Framework *framework = new Framework(this, frameworkInfo, from);

                add_framework(framework);

                message.mutable_framework_id()->MergeFrom(framework->id());
                message.mutable_master_info()->MergeFrom(m_masterinfo);

                framework->send(message);

                LOG(INFO) << "Subscribe framework " << frameworkInfo.name() << " successful!";

                const Duration temp_duration = Seconds(0);
                process::delay(temp_duration, self(), &Master::offer, framework->id());

                return;
            }
        }

        void Master::offer(const mesos::FrameworkID &frameworkId) {
            Framework *framework = CHECK_NOTNULL(frameworks.registered.at(frameworkId.value()));

            mesos::internal::ResourceOffersMessage message;

            foreachvalue(Slave* slave, slaves.registered) {

                mesos::Offer *offer = new mesos::Offer();

//            Slave *slave = find_slave_to_run();
                int memory = slave->m_runtimeinfo.mem_usage().mem_available() / 1024;
                LOG(INFO) << "Slave is " << slave->m_pid
                          << " cpu cores is " <<  slave->m_hardwareinfo.cpu_collection().cpu_quantity()
                          << " ; " << " memory avalable is " << memory << " MB";

                // cpus
                mesos::Resource *cpu_resource = new mesos::Resource();
                cpu_resource->set_name("cpus");
                cpu_resource->set_type(mesos::Value_Type_SCALAR);
                mesos::Value_Scalar *cpu_scalar = new mesos::Value_Scalar();
                cpu_scalar->set_value(slave->m_hardwareinfo.cpu_collection().cpu_quantity());
                cpu_resource->mutable_scalar()->CopyFrom(*cpu_scalar);
                offer->add_resources()->MergeFrom(*cpu_resource);

                // memory
                mesos::Resource *mem_resource = new mesos::Resource();
                mem_resource->set_name("mem");
                mem_resource->set_type(mesos::Value_Type_SCALAR);
                mesos::Value_Scalar *mem_scalar = new mesos::Value_Scalar();
                mem_scalar->set_value(memory);
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


                offer->mutable_id()->MergeFrom(new_offer_id());
                offer->mutable_framework_id()->MergeFrom(framework->id());

                offer->mutable_slave_id()->set_value(slave->m_uid);
                offer->set_hostname(slave->m_hostname);

                LOG(INFO) << "offer id " << offer->id().value();

                offers.put(offer->id().value(), offer);

                message.add_offers()->MergeFrom(*offer);
                message.add_pids(offer->id().value());
            }

            LOG(INFO) << "Sending " << message.offers().size() << " offer to framework "
                      << *framework;

            framework->send(message);
        }

        void Master::accept(Framework *framework, mesos::scheduler::Call::Accept accept) {

            foreach(const mesos::OfferID& offerId, accept.offer_ids() ) {

                LOG(INFO) << "accept_offer_ids " << accept.offer_ids().size();

                const mesos::Offer *offer = get_offer(offerId);

                Slave *slave = get_slave(offer->slave_id().value());

                //judge the operation type
                for (int i = 0; i < accept.operations_size(); ++i) {
                    mesos::Offer::Operation *operation = accept.mutable_operations(i);
                    if (operation->type() == mesos::Offer::Operation::LAUNCH) {
                        if (operation->launch().task_infos().size() > 0) {
                            LOG(INFO) << "Get offer from framework " << *framework;
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

                foreach (const mesos::Offer::Operation &operation, accept.operations()) {
                    switch (operation.type()) {
                        case mesos::Offer::Operation::LAUNCH: {
                            mesos::Offer::Operation _operation;

                            _operation.set_type(mesos::Offer::Operation::LAUNCH);

                            foreach (const mesos::TaskInfo &task, operation.launch().task_infos()) {

                                mesos::TaskInfo task_(task);

                                LOG(INFO) << "Sending task to slave " << slave->m_pid;

                                mesos::internal::RunTaskMessage message;
                                message.mutable_framework()->MergeFrom(framework->m_info);
                                message.set_pid(framework->m_pid.getOrElse(UPID()));
                                message.mutable_task()->MergeFrom(task_);
                                message.mutable_framework_id()->MergeFrom(framework->id());

                                send(slave->m_pid, message);

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
        }


        void Master::teardown_framework(Framework *framework) {
            CHECK_NOTNULL(framework);
            LOG(INFO) << "Processing TEARDOWN call for framework " << *framework;
            remove_framework(framework);
        }

        void Master::remove_framework(Framework *framework) {
            CHECK_NOTNULL(framework);

            LOG(INFO) << "Removing framework " << *framework;

            if (framework->active()) {
                CHECK(framework->active());
                LOG(INFO) << "Deactive framework " << *framework;
                framework->state = Framework::State::INACTIVE;
            }

            //如果有framework就杀掉,如果没有就跳过？？
            foreachvalue(Slave *slave, slaves.registered) {
                mesos::internal::ShutdownFrameworkMessage message;
                message.mutable_framework_id()->MergeFrom(framework->id());
                send(slave->m_pid, message);
            }
        }


        void Master::decline_framework(master::Framework *framework,
                                       const mesos::scheduler::Call::Decline &decline) {
            CHECK_NOTNULL(framework);
            LOG(INFO) << "Processing DECLINE call for offers: " << decline.offer_ids().data()
                      << " for framework " << *framework;
        }

        void Master::shutdown_slave_executor(master::Framework *framework,
                                             const mesos::scheduler::Call::Shutdown &shutdown) {
            CHECK_NOTNULL(framework);
            const mesos::SlaveID &slaveID = shutdown.slave_id();
            const mesos::ExecutorID &executorID = shutdown.executor_id();
            const mesos::FrameworkID &frameworkID = framework->id();

            Slave *slave = get_slave(slaveID.value());

            LOG(INFO) << "Processing SHUTDOWN call for executor '" << executorID.value()
                      << "' of framework " << *framework << " on agent " << slaveID.value();

            mesos::internal::ShutdownExecutorMessage message;
            message.mutable_executor_id()->CopyFrom(executorID);
            message.mutable_framework_id()->CopyFrom(frameworkID);
            send(slave->m_pid, message);
        }

        void Master::status_update(mesos::internal::StatusUpdate update, const UPID &pid) {
            LOG(INFO) << "Status update " << update.status().state()
                      << " of framework " << update.framework_id().value()
                      << " from agent " << update.slave_id().value();

            Framework *framework = get_framework(update.framework_id());

            if (update.has_uuid()) {
                update.mutable_status()->set_uuid(update.uuid());
            }

            if (update.has_framework_id()) {
                LOG(INFO) << "Forwarding status update " << update.status().state()
                          << " to framework " << update.framework_id().value();

                mesos::internal::StatusUpdateMessage message;
                message.mutable_update()->MergeFrom(update);
                message.set_pid(pid);   //this pid is slavePID
                LOG(INFO) << "StatusUpdateMessage send to "<< pid;

                framework->send(message);
            }
        }

        void Master::status_update_acknowledgement(
                const UPID &from,
                const mesos::SlaveID &slaveId,
                const mesos::FrameworkID &frameworkId,
                const mesos::TaskID &taskId,
                const string &uuid) {
            Framework *framework = get_framework(frameworkId);

            LOG(INFO) << "Status Update Acknowledgement from " << from;

            mesos::scheduler::Call::Acknowledge message;
            message.mutable_slave_id()->CopyFrom(slaveId);
            message.mutable_task_id()->CopyFrom(taskId);
            message.set_uuid(uuid);

            acknowledge(framework, message);
        }

        void Master::acknowledge(Framework *framework, const mesos::scheduler::Call::Acknowledge &acknowledge) {
            const mesos::SlaveID &slaveId = acknowledge.slave_id();

            LOG(INFO) << "acknowledge slave_id" << acknowledge.slave_id().value();

            Slave *slave = get_slave(slaveId.value());

            const mesos::TaskID &taskId = acknowledge.task_id();
            const UUID uuid = UUID::fromBytes(acknowledge.uuid()).get();

            mesos::internal::StatusUpdateAcknowledgementMessage message;
            message.mutable_slave_id()->CopyFrom(slaveId);

            message.mutable_framework_id()->MergeFrom(framework->id());
            message.mutable_task_id()->CopyFrom(taskId);
            message.set_uuid(uuid.toBytes());


            LOG(INFO) << "Processing ACKNOWLEDGE call " << uuid << " for task " << taskId.value()
                      << " of framework " << framework->m_info.name() << " on agent " << slaveId.value();
            send(slave->m_pid, message);
        }

        Slave *Master::find_slave_to_run() {

            int i = 0;
            double min_use_rate = 100;
            double cur_mem_rate;

            foreachvalue(Slave *slave, slaves.registered) {

                RuntimeResourcesMessage rrm = m_slave_usage.get(slave->m_pid).get();

                cur_mem_rate = rrm.mem_usage().mem_available() / rrm.mem_usage().mem_total();

                if (min_use_rate > cur_mem_rate) {
                    min_use_rate = cur_mem_rate;
                    m_slave_pid = slave->m_pid;
                }
            }
            LOG(INFO) << "Choose " << m_slave_pid << " offer resource to framework";
            return slaves.registered.get(m_slave_pid);
        }

        mesos::FrameworkID Master::new_framework_id() {
            std::ostringstream out;
            out << m_masterinfo.id() << "-" << std::setw(4)
                << std::setfill('0') << m_next_framework_id++;
            mesos::FrameworkID frameworkId;
            frameworkId.set_value(out.str());
            return frameworkId;
        }

        mesos::OfferID Master::new_offer_id() {
            mesos::OfferID offerId;
            offerId.set_value(m_masterinfo.id() + "-O" + stringify(m_next_offer_id++));
            return offerId;
        }

        void Master::add_slave(master::Slave *slave) {
            CHECK_NOTNULL(slave);
            CHECK(!slaves.registered.contains(slave->m_uid));
            slaves.registered.put(slave);
            link(slave->m_pid);
        }

        void Master::add_framework(Framework *framework) {

            frameworks.registered[framework->id().value()] = framework;

            if (framework->connected()) {
                if (framework->m_pid.isSome()) {
                    link(framework->m_pid.get());
                }
            }
        }

        Slave *Master::get_slave(const string uid) {
            return slaves.registered.contains(uid)
                   ? slaves.registered.get(uid)
                   : nullptr;
        }

        mesos::Offer *Master::get_offer(const mesos::OfferID &offerid) {
            return offers.contains(offerid.value())
                   ? offers.get(offerid.value()).get()
                   : nullptr;
        }

        Framework *Master::get_framework(const mesos::FrameworkID &frameworkId) {
            return frameworks.registered.contains(frameworkId.value())
                   ? frameworks.registered.at(frameworkId.value())
                   : nullptr;
        }

        void Master::register_participant(const string &hostname) {
            DLOG(INFO) << "master receive register message from " << hostname;
        }

        void Master::update_hardware_resources(const UPID &from,
                                               const HardwareResourcesMessage &hardware_resources_message) {
            DLOG(INFO) << "Enter update hardware resources from " << from;

            //save slaveinfo - by weiguow
            slaves.registering.insert(from);

            Slave *slave = new Slave(this,
                                     hardware_resources_message,
                                     hardware_resources_message.slave_uuid(),
                                     hardware_resources_message.slave_hostname(),
                                     from);

            add_slave(slave);

            LOG(INFO) << "Slave " << *slave << " registered on " << self() << " successful!";

            auto slave_id = hardware_resources_message.slave_id();
            if (m_hardware_resources.find(slave_id) == m_hardware_resources.end()) {
                JSON::Object object = JSON::protobuf(hardware_resources_message);
                m_hardware_resources.insert({slave_id, object});
                m_proto_hardware_resources.insert({slave_id, hardware_resources_message});
                m_alive_slaves.insert(slave_id);
            }
        }

        void Master::received_heartbeat(const UPID &from, const RuntimeResourcesMessage &runtime_resouces_message) {
            LOG(INFO) << "received a heartbeat message from " << from;

            //save runtimeinfo-by weiguow
            foreachvalue(Slave *slave, slaves.registered) {
                if (!slaves.registered.contains(slave->m_uid)) {
                                        LOG(INFO) << "This slave is not registered";
                                        return;
                }
                Slave *slave_ = get_slave(slave->m_uid);
                if (!m_slave_usage.contains(slave->m_pid)) {
                    m_slave_usage.put(slave->m_pid, runtime_resouces_message);
                    slave->m_runtimeinfo = runtime_resouces_message;
                }
            }

            auto slave_id = runtime_resouces_message.slave_id();
            m_runtime_resources[slave_id] = JSON::protobuf(runtime_resouces_message);
            m_proto_runtime_resources[slave_id] = runtime_resouces_message;
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
        Master::received_terminating_master_message(const UPID &super_master,
                                                    const TerminatingMasterMessage &message) {
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

        void Master::get_select_master(const UPID &from, const string &message) {
            LOG(INFO) << "MAKUN received select_master_message";
            send(from, "MAKUN2");
        }

        // end of super_mater related
//    std::ostream &operator<<(std::ostream &stream, const mesos::TaskState &state)
    }
}

int main(int argc, char **argv) {
    chameleon::set_storage_paths_of_glog("master");// provides the program name
    chameleon::set_flags_of_glog();

    google::SetUsageMessage("usage : Option[name] \n"
                            "--port     the port used by the program");
    google::SetVersionString("Chameleon v1.0");
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::CommandLineFlagInfo info;

    if (has_port_Int && has_super_master_path) {
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
    return 0;
}
