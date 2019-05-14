//
// Created by root on 19-5-6.
//

#include <string>

#include <process/pid.hpp>
#include <process/process.hpp>

#include <stout/os.hpp>
#include <stout/uuid.hpp>

#include "detector.hpp"

using std::string;
using namespace process;

namespace mesos{
    namespace master{
        namespace detector{

            Try<MasterDetector*> MasterDetector::create(const Option<std::string> &zk_,
                                                        const Option<std::string> &masterDetectorModule,
                                                        const Option<Duration> &zkSessionTimeout) {
                LOG(INFO) << "MasterDetector is creating";
                if(zk_.isNone()){
                    return new StandaloneMasterDetector();
                }
                const string& zk = zk_.get();

                UPID pid = zk.find("master@") == 0
                           ? UPID(zk)
                           : UPID("master@" + zk);

                //createMasterInfo
                mesos::MasterInfo info;
                info.set_id(stringify(pid) + "-" + UUID::random().toString());

                info.set_ip(pid.address.ip.in().get().s_addr);
                info.set_port(pid.address.port);

                info.mutable_address()->set_ip(stringify(pid.address.ip));
                info.mutable_address()->set_port(pid.address.port);

                info.set_pid(pid);

                Try<string> hostname = net::getHostname(pid.address.ip);
                if (hostname.isSome()) {
                    info.set_hostname(hostname.get());
                    info.mutable_address()->set_hostname(hostname.get());
                }

                return new StandaloneMasterDetector(info);
            }

            MasterDetector::~MasterDetector(){}

        }//namespace detector
    }//namespace master
}//namespace mesos