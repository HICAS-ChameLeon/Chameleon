/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-1-15
 * Description：super_master
 */

#include <super_master_related.pb.h>
#include "super_master.hpp"
#include "master.hpp"

DEFINE_string(master_path, "", "the absolute path of master executive. For example, --master_path=/home/lemaker/open-source/Chameleon/build/src/master/master");
DEFINE_string(initiator, "localhost:6060", "the ip:port of the current master of first level or supermaster. For example, --initiator=172.20.110.228:6060");

static bool ValidateStr(const char *flagname, const string &value) {
    if (!value.empty()) {
        return true;
    }
    printf("Invalid value for --%s: To run this program, you must set a meaningful value for it "
           "%s\n", flagname, value.c_str());;
    return false;
}
static const bool has_master_path = gflags::RegisterFlagValidator(&FLAGS_master_path, &ValidateStr);
static const bool has_initiator = gflags::RegisterFlagValidator(&FLAGS_initiator, &ValidateStr);

namespace chameleon {
    void SuperMaster::initialize() {
        // Verify that the version of the library that we linked against is
        // compatible with the version of the headers we compiled against.
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        install<MasterRegisteredMessage>(&SuperMaster::registered_master);
        install<OwnedSlavesMessage>(&SuperMaster::terminating_master);
        //kill_master related
        install("KILL",&SuperMaster::owned_masters_message);
        //kill_master end
        install("MAKUN2",&SuperMaster::recevied_slave_infos);

        // change from one level to two levels
        cluster_levels = 2;
        m_masters_size = 1;

        m_uuid = UUID::random().toString();
        const string initiator_pid = "master@"+m_initiator;
        SuperMasterControlMessage *super_master_control_message = new SuperMasterControlMessage();
        super_master_control_message->set_super_master_id(initiator_pid);
        super_master_control_message->set_super_master_uuid(m_uuid);
        super_master_control_message->set_passive(false);

        UPID t_master(initiator_pid);
        send(t_master, *super_master_control_message);
        LOG(INFO) << " sends a super_master_constrol_message to a master: " << initiator_pid;
//        delete super_master_control_message;



        route(
                "/super_slave",
                "start super_master and change from one level to two levels",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    if (!this->m_classification_slaves.empty()) {
                        JSON::Array array;
                        for (auto it = this->m_classification_slaves.begin();
                             it != this->m_classification_slaves.end(); it++) {
                            vector<SlavesInfoControlledByMaster> slaveinfo = it->second;
                            for (int j = 0; j < slaveinfo.size(); ++j) {
                                array.values.push_back(JSON::protobuf(slaveinfo[j]));
                            }

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
                "/super_master",
                "start super_master and change from one level to two levels",
                [this](Request request) {
                    JSON::Object result = JSON::Object();
                    if (!this->m_classification_masters.empty()) {
                        JSON::Array array;
                        for (auto it = this->m_classification_masters.begin();
                             it != this->m_classification_masters.end(); it++) {
                            for (int j = 0; j < m_classification_masters.size(); ++j) {

                                //JSON::Object result2 = JSON::Object(stringify(result));
                                result.values["ip"] = m_classification_masters[0];
                                //array.values.push_back(JSON::String(m_classification_masters[j]));
                                array.values.push_back(result);
                            }

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


        route(  //change from two level to one levels
                "/kill_master",
                "kill the super_master of two levels",
                [this](Request request){
                    JSON::Object result = JSON::Object();
                    LOG(INFO) << "MAKUN KILL MASTER";
                    string new_master_ip = select_master();
                    result.values["new_master_ip"] = new_master_ip;
                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    //select_master();

                    return ok_response;
                });


    }

    void SuperMaster::registered_master(const UPID &from, const MasterRegisteredMessage &master_registered_message) {
        LOG(INFO) << "accept a mater_registered_message from " << from;
        Future<bool> distinctive = true;
        distinctive = distinctive.then(defer(self(), &Self::is_repeated_registered, from));
        distinctive.onAny(defer(self(), &Self::record_master, lambda::_1, from, master_registered_message));

    }

    void SuperMaster::set_master_path(const string& path) {
        m_master_path = path;
    }

    void SuperMaster::set_first_to_second_master(const string &master) {

    }

    Future<bool> SuperMaster::is_repeated_registered(const UPID &upid) {
        if (std::find(m_masters.begin(), m_masters.end(), upid) != m_masters.end()) {
            LOG(INFO) << " master " << upid << " registered repeatedly!";

            return false;
        }
        return true;
    }

    void SuperMaster::record_master(const Future<bool> &future, const UPID &from,
                                    const MasterRegisteredMessage &master_registered_message) {
        CHECK(!future.isDiscarded());
        AcceptRegisteredMessage *accept_registered = new AcceptRegisteredMessage();
        accept_registered->set_master_id(stringify(from.address.ip));

        if (!future.isReady()) {
            accept_registered->set_status(AcceptRegisteredMessage_Status_FAILURE);
            send(from, *accept_registered);
            delete accept_registered;
            LOG(ERROR) << "Failed to record master for this super master due to "
                       << (future.isFailed() ? future.failure() : "future discarded");

            return;
        }
        if (!future.get()) {
            accept_registered->set_status(AcceptRegisteredMessage_Status_FAILURE);
            send(from, *accept_registered);
            delete accept_registered;
            LOG(INFO) << " master registered repeatedly!";
            return;
        }

        m_masters.push_back(from);
        LOG(INFO) << "record a registered master " << from <<" Now the size of masters is "<<m_masters.size();
        accept_registered->set_status(AcceptRegisteredMessage_Status_SUCCESS);
        send(from, *accept_registered);
        delete accept_registered;
        return;
    }

    void SuperMaster::terminating_master(const UPID &from, const OwnedSlavesMessage &message) {
        LOG(INFO) << " get an OwnedSlavesMessage from " << from;
        LOG(INFO) << "MAKUN slaveInfo size: " << message.slave_infos().size();

        std::copy(message.slave_infos().begin(), message.slave_infos().end(), std::back_inserter(m_admin_slaves));
        LOG(INFO) << "MAKUN admin slave size: " << m_admin_slaves.size();
        for (SlaveInfo &slaveInfo: m_admin_slaves) {
            LOG(INFO) << "MAKUN slaveInfo has " << slaveInfo.hardware_resources().cpu_collection().cpu_infos_size() << "CPU";
        }
        TerminatingMasterMessage *terminating_master = new TerminatingMasterMessage();
        terminating_master->set_master_id(stringify(from.address.ip));
        LOG(INFO) << "MAKUN: " << from;
        send(from, *terminating_master);
        delete terminating_master;
        LOG(INFO) << " send a TerminatingMasterMessage to master " << from
                  << " since the super master has receive the owned slaves of that master";

        // delete the terminating_master from m_masters
        auto iter = std::find(m_masters.begin(), m_masters.end(), from);
        if (iter != m_masters.end()) {
            m_masters.erase(iter);
        }

        // create new masters
        process::delay(Seconds(1), self(), &Self::create_masters);

    }

    void SuperMaster::classify_masters() {

        CHECK(m_masters.size() == 0);
        m_classification_slaves.clear();
        m_classification_masters.clear();

        // first algorithm, classification by the number of ips
        int32_t count = 1;
        int32_t cluster_size = m_admin_slaves.size() / m_masters_size;

        int32_t current_cluster_number = 0;

        string master_ip;
        for (auto iter = m_admin_slaves.begin(); iter != m_admin_slaves.end(); iter++) {
            SlaveInfo &current_slave = *iter;
            string current_ip = current_slave.hardware_resources().slave_id();
//            int32_t cpus = current_slave.hardware_resources().cpu_collection().cpu_infos_size();

            if(count > cluster_size){
                count = 1;
            }
            if (count == 1) {
                current_cluster_number++;
                if(current_cluster_number>m_masters_size){
                    SlavesInfoControlledByMaster a_slave;
                    a_slave.set_ip(current_ip);
                    a_slave.set_port("6061");
                    m_classification_slaves[master_ip].push_back(a_slave);
                    break;
                }
                master_ip = current_ip;
                m_classification_masters.push_back(master_ip);
                m_classification_slaves.insert({master_ip, vector<SlavesInfoControlledByMaster>()});
            }

            SlavesInfoControlledByMaster a_slave;
            a_slave.set_ip(current_ip);
            a_slave.set_port("6061");
            m_classification_slaves[master_ip].push_back(a_slave);
            count++;

        }

        for(auto iter = m_classification_masters.begin();iter!=m_classification_masters.end();iter++){
            vector<SlavesInfoControlledByMaster> slaves_of_master = m_classification_slaves[*iter];
            std::cout<<slaves_of_master.size()<<std::endl;
//            LOG(INFO) << slaves_of_master.size();
            for(SlavesInfoControlledByMaster s:slaves_of_master){
                std::cout<<s.ip()<<std::endl;
//                LOG(INFO) << s.ip();
            }
        }
    }

    bool SuperMaster::launch_masters() {
//        Try<Subprocess> s = subprocess(
//                "/home/lemaker/open-source/Chameleon/build/src/master/master --port=6060",
//                Subprocess::FD(STDIN_FILENO),
//                Subprocess::FD(STDOUT_FILENO),
//                Subprocess::FD(STDERR_FILENO));

        for(const string& master_ip:m_classification_masters){
            string ssh_command = "ssh "+master_ip+" "+m_master_path+" --port=6060";
            Try<Subprocess> s = subprocess(
                    ssh_command,
                    Subprocess::FD(STDIN_FILENO),
                    Subprocess::FD(STDOUT_FILENO),
                    Subprocess::FD(STDERR_FILENO));

            if (s.isError()) {
                LOG(ERROR) << " cannot launch master "<<master_ip;
                return false;
            }
        }
        LOG(INFO)<<" launched "<<m_classification_masters.size() << " masters successfully.";
        return true;
//        Try<Subprocess> s = subprocess(
//                "ssh 172.20.110.228 /home/lemaker/open-source/Chameleon/build/src/master/master --port=6060",
//                Subprocess::FD(STDIN_FILENO),
//                Subprocess::FD(STDOUT_FILENO),
//                Subprocess::FD(STDERR_FILENO));



    }

    void SuperMaster::create_masters(){
        classify_masters();
        bool launch_success = launch_masters();
        if(launch_success){
            LOG(INFO)<<" launched all new masters successfully!";
            process::delay(Seconds(3), self(), &Self::send_super_master_control_message);
        }else{
            LOG(INFO) << "launching masters failed!";
        }
    }

    void SuperMaster::send_super_master_control_message(){
        for(const string& master_ip: m_classification_masters){
            SuperMasterControlMessage *super_master_control_message = new SuperMasterControlMessage();
            super_master_control_message->set_super_master_id(master_ip);

            super_master_control_message->set_passive(true);
            for(const SlavesInfoControlledByMaster& slave_info: m_classification_slaves[master_ip]){
                SlavesInfoControlledByMaster* new_slave = super_master_control_message->add_my_slaves();
                new_slave->CopyFrom(slave_info);
            }
            string master_upid = "master@"+master_ip+":6060";
            UPID t_master(master_upid);
            send(t_master, *super_master_control_message);
            LOG(INFO) << " sends a super_master_control_message to a master: " << master_upid;
            delete super_master_control_message;
        }
    }

    //kill_master related
    void SuperMaster::owned_masters_message(const UPID& kill_master, const string& name){
        LOG(INFO) << "MAKUN RECIEVED KILL MESSAGE";
        //OwnedSlavesMessage *owned_slaves = new OwnedSlavesMessage();
        m_owned_slaves_message = new OwnedSlavesMessage();

        SlaveInfo *t_slave = m_owned_slaves_message->add_slave_infos();
        m_owned_slaves_message->set_quantity(m_owned_slaves_message->slave_infos_size());
        send(kill_master, *m_owned_slaves_message);
        //delete m_owned_slaves_message;
        LOG(INFO) << " send owned slaves of " << self() << " to kill_master " << kill_master;
    }
    //kill_master end

    const string SuperMaster::select_master(){
        string master_ip;
        int num_slaves = 0;
        for(auto iter = m_classification_masters.begin();iter!=m_classification_masters.end();iter++){
            vector<SlavesInfoControlledByMaster> slaves_of_master = m_classification_slaves[*iter];
            if(num_slaves<slaves_of_master.size()) {
                num_slaves = slaves_of_master.size();
                master_ip = *iter;
            }
        }
        LOG(INFO) << "MAKUN select master ip: " << master_ip;
        send_terminating_master(master_ip);
        return master_ip;
    }
    void SuperMaster::send_terminating_master(string master_ip) {
        //master_ip = "master@"+master_ip+":6060";
//        UPID master_upid(master_ip);
        TerminatingMasterMessage *terminating_master = new TerminatingMasterMessage();
        terminating_master->set_master_id(master_ip);
        for (auto iter = m_classification_masters.begin(); iter != m_classification_masters.end(); iter++) {
            if (*iter != master_ip) {
                *iter = "master@"+*iter+":6060";
                send(*iter, *terminating_master);
            } else {
                UPID master_upid("master@"+master_ip+":6060");
                send(master_upid,"MAKUN");
            }
        }
        LOG(INFO) << self() << " is terminating due to change levels to one";
        delete(terminating_master);
    }
    void SuperMaster::recevied_slave_infos(const UPID& from, const string& message){
        LOG(INFO) << "MAKUN received message from new master";
        terminate(self());
//        process::wait(self());
    }

    //framework related
    void SuperMaster::receive(const UPID &from, const mesos::scheduler::Call &call) {
        //first call
        if (call.type() == mesos::scheduler::Call::SUBSCRIBE) {
            subscribe(from, call.subscribe());
            return;
        }

        master::Framework *framework = getFramework(call.framework_id());

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

    void SuperMaster::subscribe(const UPID &from, const mesos::scheduler::Call::Subscribe &subscribe) {

        mesos::FrameworkInfo frameworkInfo = subscribe.framework_info();

        master::Framework *framework = getFramework(frameworkInfo.id());

        LOG(INFO) << "Received  SUBSCRIBE call for framework "
                  << frameworkInfo.name() << " at " << from;

        if (!frameworkInfo.has_id() || frameworkInfo.id().value().empty()) {

            // If we are here the framework is subscribing for the first time.
            // Check if this framework is already subscribed (because it retries).
            foreachvalue (master::Framework *framework, frameworks.registered) {
                                    if (framework->pid == from) {
                                        LOG(INFO) << "Framework " << *framework
                                                  << " already subscribed, resending acknowledgement";
                                        mesos::internal::FrameworkRegisteredMessage message;
                                        message.mutable_framework_id()->MergeFrom(framework->id());
//                                        message.mutable_master_info()->MergeFrom(framework->master->m_masterInfo);
                                        framework->send(message);
                                        return;
                                    }
                                }

            mesos::internal::FrameworkRegisteredMessage message;

            frameworkInfo.mutable_id()->CopyFrom(newFrameworkId());
//            master::Framework *framework = new Framework(this, frameworkInfo, from);

//            addFramework(framework);

//            message.mutable_framework_id()->MergeFrom(framework->id());
//            message.mutable_master_info()->MergeFrom(m_masterInfo);

//            framework->send(message);

            LOG(INFO) << "Subscribe framework " << frameworkInfo.name() << " successful!";
//
//            const Duration temp_duration = Seconds(0);
//            //
//            process::delay(temp_duration, self(), &Master::Offer, framework->id());
// after subscribed, the framework can be given resource offers.
//            Offer(framework->id());

            return;
        }
    }

    void SuperMaster::Offer(const mesos::FrameworkID &frameworkId) {

        master::Framework *framework = CHECK_NOTNULL(frameworks.registered.at(frameworkId.value()));

        mesos::internal::ResourceOffersMessage message;

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

        //
//        offer->mutable_id()->MergeFrom(newOfferId());
        offer->mutable_framework_id()->MergeFrom(framework->id());

        //这个slaveID决定了实现master选取分布式集群中节点的基础
        mesos::SlaveID *slaveID = new mesos::SlaveID();
//        offer->mutable_slave_id()->MergeFrom(newSlaveId());
        slaveID->set_value("44444444");
        offer->mutable_slave_id()->MergeFrom(*slaveID);

        //this host_name is slave hostname
        offer->set_hostname(self().address.hostname().get());

        message.add_offers()->MergeFrom(*offer);
        message.add_pids("1");

//        mesos::Offer *second_offer = create_a_offer(framework->id());
//        message.add_offers()->MergeFrom(*second_offer);
//        message.add_pids("2");


        LOG(INFO) << "Sending " << message.offers().size() << " offer to framework "
                  << framework->pid.get();

        framework->send(message);

        return;
    }

    master::Framework *SuperMaster::getFramework(const mesos::FrameworkID &frameworkId) {
        return frameworks.registered.contains(frameworkId.value())
               ? frameworks.registered.at(frameworkId.value())
               : nullptr;
    }

    void SuperMaster::teardown(master::Framework *framework) {
        CHECK_NOTNULL(framework);

        LOG(INFO) << "Processing TEARDOWN call for framework " << *framework;

        removeFramework(framework);
    }

    void SuperMaster::removeFramework(master::Framework *framework) {
        CHECK_NOTNULL(framework);

        LOG(INFO) << "Removing framework " << *framework;

        if (framework->active()) {
            CHECK(framework->active());

            LOG(INFO) << "Deactive framework " << *framework;

            framework->state = master::Framework::State::INACTIVE;
        }
        //send ShutdownFrameworkMessage to slave
        mesos::internal::ShutdownFrameworkMessage message;
        message.mutable_framework_id()->MergeFrom(framework->id());

//        string slave_pid = "slave@172.20.110.228:6061";
        const string slave_pid = stringify(self().address.ip).append(":6061");
        send(slave_pid, message);

//        frameworks.completed.set(framework->id().value(), framework);
    }

    void SuperMaster::accept(master::Framework *framework, mesos::scheduler::Call::Accept accept) {
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

                        string cur_slavePID = "slave@";
                        if (task.slave_id().value() == "11111111") {
//                            cur_slavePID.append("172.20.110.228:6061");
                            cur_slavePID.append(stringify(self().address.ip)+":6061");
                        } else {
                            cur_slavePID.append(stringify(self().address.ip)+":6061");
                        }
                        mesos::TaskInfo task_(task);

                        LOG(INFO) << "Sending task to slave " << cur_slavePID; //slave(1)@172.20.110.152:5051

                        mesos::internal::RunTaskMessage message;
                        message.mutable_framework()->MergeFrom(framework->info);
                        message.set_pid(framework->pid.getOrElse(UPID()));
                        message.mutable_task()->MergeFrom(task_);
                        message.mutable_framework_id()->MergeFrom(framework->id());

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

    void SuperMaster::decline(master::Framework *framework, const mesos::scheduler::Call::Decline &decline) {
        CHECK_NOTNULL(framework);

        LOG(INFO) << "Processing DECLINE call for offers: " << decline.offer_ids().data()
                  << " for framework " << *framework;

        //we should save offer infomation before do this , so we now just leave it- by weiguow
//        offers.erase(offer->id());
//        delete offer;
    }

    void SuperMaster::shutdown(master::Framework *framework, const mesos::scheduler::Call::Shutdown &shutdown) {
        CHECK_NOTNULL(framework);

        const mesos::SlaveID &slaveID = shutdown.slave_id();

//        const
    }

    void SuperMaster::acknowledge(master::Framework *framework, const mesos::scheduler::Call::Acknowledge &acknowledge) {
        const mesos::SlaveID &slaveId = acknowledge.slave_id();
        const mesos::TaskID &taskId = acknowledge.task_id();
        const UUID uuid = UUID::fromBytes(acknowledge.uuid()).get();

        mesos::internal::StatusUpdateAcknowledgementMessage message;
        message.mutable_slave_id()->CopyFrom(slaveId);

        message.mutable_framework_id()->MergeFrom(framework->id());
        message.mutable_task_id()->CopyFrom(taskId);
        message.set_uuid(uuid.toBytes());

        LOG(INFO) << "Processing ACKNOWLEDGE call " << uuid << " for task " << taskId.value()
                  << " of framework " << framework->info.name() << " on agent " << slaveId.value();
        //send(m_slavePID, message);
    }

    mesos::FrameworkID SuperMaster::newFrameworkId() {
        std::ostringstream out;
//        out << m_masterInfo.id() << "-" << std::setw(4)
//            << std::setfill('0') << nextFrameworkId++;

        mesos::FrameworkID frameworkId;
        frameworkId.set_value(out.str());

        return frameworkId;
    }

    void SuperMaster::addFramework(master::Framework *framework) {

        frameworks.registered[framework->id().value()] = framework;

        if (framework->connected()) {
            if (framework->pid.isSome()) {
                link(framework->pid.get());
            }
        }
    }


}
using namespace chameleon;


int main(int argc, char **argv) {
    chameleon::set_storage_paths_of_glog("super_master");// provides the program name
    chameleon::set_flags_of_glog();

    google::SetUsageMessage("usage : Option[name] \n"
                            "--port     the port used by the program");
    google::SetVersionString("Chameleon v1.0");
    google::ParseCommandLineFlags(&argc, &argv, true);

    google::CommandLineFlagInfo info;

    if(has_master_path && has_initiator){
        os::setenv("LIBPROCESS_PORT", "7000");
        process::initialize("super_master");

        SuperMaster super_master(FLAGS_initiator);
        super_master.set_master_path(FLAGS_master_path);

        PID<SuperMaster> cur_super_master = process::spawn(super_master);
        LOG(INFO) << "Running super_master on " << process::address().ip << ":" << process::address().port;

        const PID<SuperMaster> super_master_pid = super_master.self();
        LOG(INFO) << super_master_pid;

//    bool res = super_master.launch_masters();
//    if (res) {
//        std::cout << " successfully launched master";
//    }
        process::wait(super_master.self());
    }else{
        LOG(INFO) << "To run this program , must set all parameters correctly "
                     "\n read the notice " << google::ProgramUsage();
    }


    return 0;
}
