/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：19-1-15
 * Description：super_master
 */

#include <super_master_related.pb.h>
#include "super_master.hpp"
#include "master.hpp"
#include <stack>

DEFINE_string(master_path, "", "the absolute path of master executive. For example, --master_path=/home/lemaker/open-source/Chameleon/build/src/master/master");
DEFINE_string(initiator, "localhost:6060", "the ip:port of the current master of first level or supermaster. For example, --initiator=172.20.110.228:6060");
DEFINE_string(webui_path, "", "the absolute path of webui. For example, --webui=/home/lemaker/open-source/Chameleon/src/webui");

static bool ValidateStr(const char *flagname, const string &value) {
    if (!value.empty()) {
        return true;
    }
    printf("Invalid value for --%s: To run this program, you must set a meaningful value for it "
           "%s\n", flagname, value.c_str());;
    return false;
}

static bool validate_webui_path(const char *flagname, const string &value) {

    if (os::exists(value)) {
        return true;
    }
    printf("Invalid value for webui_path, please make sure the webui_path actually exist!\n");
    return false;

}
static const bool has_master_path = gflags::RegisterFlagValidator(&FLAGS_master_path, &ValidateStr);
static const bool has_initiator = gflags::RegisterFlagValidator(&FLAGS_initiator, &ValidateStr);
static const bool has_webui_path = gflags::RegisterFlagValidator(&FLAGS_webui_path, &validate_webui_path);


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
        install<>(&SuperMaster::received_acknowledgement);

        //franework related
        install<mesos::scheduler::Call>(&SuperMaster::received_call);
        install<mesos::internal::FrameworkRegisteredMessage>(&SuperMaster::received_registered);
        install<mesos::internal::ResourceOffersMessage>(&SuperMaster::received_resource);
        install<mesos::internal::StatusUpdateMessage>(&SuperMaster::received_status);
        //install<mesos::internal::StatusUpdateAcknowledgementMessage>(&SuperMaster::received_acknowledgement);
        install("error",&SuperMaster::launch_master_results);
        install("successed",&SuperMaster::launch_master_results);

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
                    result.values["stop"] = "success";
                    //result.values["new_master_ip"] = new_master_ip;
                    OK ok_response(stringify(result));
                    ok_response.headers.insert({"Access-Control-Allow-Origin", "*"});
                    //select_master();

                    return ok_response;
                });


    }

    const string SuperMaster::get_cwd() {
        return m_super_master_cwd;
    }

    void SuperMaster::set_webui_path(const string &path)  {
        m_webui_path = path;
    }

    const string SuperMaster::get_web_ui() const {
        return m_webui_path;
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
        classify_masters_framework();
    }

    // launch the exectuables of maters administered by the current super_master
    void SuperMaster::launch_masters() {
        LaunchMasterMessage *launch_master_message = new LaunchMasterMessage();
        launch_master_message->set_port("6060");
        launch_master_message->set_master_path(m_master_path);
        launch_master_message->set_webui_path(m_webui_path);
        for(const string& master_ip:m_classification_masters) {
            send(UPID("slave@" + master_ip + ":6061"), *launch_master_message);
            LOG(INFO) << "send message to " << master_ip;
            // since the both the master executable and super_master executable are in the same directory,
            // so we get the current directory path of super_master exectuable to stands for the path of cd command"
//            string launch_master = " cd "+get_cwd()+" && " + m_master_path+" --port=6060";
//            string ssh_command = "ssh "+master_ip+launch_master;
//            Try<Subprocess> s = subprocess(
//                    ssh_command,
//                    Subprocess::FD(STDIN_FILENO),
//                    Subprocess::FD(STDOUT_FILENO),
//                    Subprocess::FD(STDERR_FILENO));
//            if (s.isError()) {
//                LOG(ERROR) << " cannot launch master "<<master_ip;
//                return false;
//            }
        }
//        if (is_launch_master == false){
//            return false;
//        }
//        LOG(INFO)<<" launched "<<m_classification_masters.size() << " masters successfully.";
//        return true;
    }
    void SuperMaster::is_launch() {
        if(is_launch_master){
            LOG(INFO)<<" launched "<<m_classification_masters.size() << " masters.";
            LOG(INFO)<<" launched all new masters successfully!";
            send_super_master_control_message();
        }else{
            LOG(INFO) << "launching masters failed!";
        }
    }

    void SuperMaster::create_masters(){
        classify_masters();
//        bool launch_success = launch_masters();
        launch_masters();
        process::delay(Seconds(3),self(),&Self::is_launch);
//        if(is_launch_master){
//            LOG(INFO)<<" launched all new masters successfully!";
//            process::delay(Seconds(3), self(), &Self::send_super_master_control_message);
//        }else{
//            LOG(INFO) << "launching masters failed!";
//        }
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
    void SuperMaster::received_call(const UPID &from, const mesos::scheduler::Call &call) {
        mesos::MasterInfo *master_info = new mesos::MasterInfo();
        for(auto iter = m_classification_masters_framework.begin();
            iter != m_classification_masters_framework.end(); iter++){
            if (iter->first.find("spark") != string::npos) {
                master_info->set_pid("master@"+iter->second+":6060");
                vector<string> master_ip = strings::tokenize(iter->second,".");
                unsigned int master_ip_int = std::stoi(master_ip[0])+256*(std::stoi(master_ip[1])+
                        256*(std::stoi(master_ip[2])+256*(std::stoi(master_ip[3]))));
                LOG(INFO)<<master_ip_int;
                master_info->set_ip(master_ip_int);
                master_info->set_port(6060);
                master_info->set_id("111622f1-1e63-456e-8fc5-e64ebb30fcb8-0000");
                send(from,*master_info);
                LOG(INFO)<<"send MasterInfo";
                break;
            }
        }
//        LOG(INFO) << "MAKUN Supermaster received call from " << from;
////        Framework *framework = getFramework(call.framework_id());
//        LOG(INFO)<<"*********************"<<call.framework_id().value();
//        m_framework = from;
//        for(auto iter = m_classification_masters_framework.begin();
//            iter != m_classification_masters_framework.end(); iter++){
//            if (iter->first.find("spark") != string::npos) {
//                send(UPID("master@" + iter->second + ":6060"),call);
//                LOG(INFO) << "MAKUN send call to master: master@" << iter->second << ":6060";
//                break;
//            }
//        }
    }

    void SuperMaster::received_registered(const UPID &from, const mesos::internal::FrameworkRegisteredMessage &message) {
        LOG(INFO) << "MAKUN Supermaster received frameworkRegistered from " << from;
        send(m_framework,message);
        LOG(INFO) << "MAKUN send frameworkRegistered to " << m_framework;
    }

    void SuperMaster::received_resource(const UPID &from, const mesos::internal::ResourceOffersMessage &message) {
        LOG(INFO) << "MAKUN Supermaster received resourceOffers from " << from;
        send(m_framework,message);
        LOG(INFO) << "MAKUN send resourceOffers to " << m_framework;
    }

    void SuperMaster::received_status(const UPID &from, const mesos::internal::StatusUpdateMessage &message) {
        LOG(INFO) << "MAKUN received statusUpdate from " << from;
        send(m_framework,message);
        LOG(INFO) << "MAKUN send statusUpdate to " << m_framework;
    }

    void SuperMaster::received_acknowledgement(const UPID &from,
            const mesos::internal::StatusUpdateAcknowledgementMessage &message) {
        LOG(INFO) << "MAKUN received statusUpdateAcknowledgement from " << from;
        for(auto iter = m_classification_masters_framework.begin();
            iter != m_classification_masters_framework.end(); iter++){
            if (iter->first.find("spark") != string::npos) {
                send(UPID("master@" + iter->second + ":6060"),message);
                LOG(INFO) << "MAKUN send message to master: master@" << iter->second << ":6060";
                break;
            }
        }
    }

    void SuperMaster::classify_masters_framework() {
        if(m_classification_masters.size() > 1){
            m_classification_masters_framework.insert(std::pair<string,string>("spark",m_classification_masters[0].data()));
            m_classification_masters_framework.insert(std::pair<string,string>("flink",m_classification_masters[1].data()));
        } else m_classification_masters_framework.insert(std::pair<string,string>("spark,flink",m_classification_masters[0].data()));
    }

    void SuperMaster::launch_master_results(const UPID &from, const string &message) {
        if(message == "error"){
            LOG(ERROR) << from << " cannot launch master";
            is_launch_master = false;
        }
        LOG(INFO) << from << " launched master successfully.";
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

    if(has_master_path && has_initiator && has_webui_path){
        os::setenv("LIBPROCESS_PORT", "7000");
        process::initialize("super_master");

        SuperMaster super_master(FLAGS_initiator);
        super_master.set_master_path(FLAGS_master_path);
        super_master.set_webui_path(FLAGS_webui_path);

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
