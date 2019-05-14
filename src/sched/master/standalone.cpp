//
// Created by root on 19-5-6.
//

#include <set>

#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/id.hpp>
#include <process/process.hpp>

#include "standalone.hpp"
#include "detector.hpp"

using namespace process;

namespace mesos{
    namespace master{
        namespace detector{
            class StandaloneMasterDetectorProcess : public Process<StandaloneMasterDetectorProcess>{
            public:
                StandaloneMasterDetectorProcess() : ProcessBase(ID::generate("standalone-master-detector")){}

                explicit  StandaloneMasterDetectorProcess(const MasterInfo& _leader)
                        : ProcessBase(ID::generate("standalone-master-detector")), leader(_leader){}

                ~StandaloneMasterDetectorProcess() {
                    discardPromises(&promises);
                }

            private:
                Option<MasterInfo> leader; // The appointed master.
                set<Promise<Option<MasterInfo>>*> promises;
            };

            StandaloneMasterDetector::StandaloneMasterDetector()
            {
                process = new StandaloneMasterDetectorProcess();
                spawn(process);
            }

            StandaloneMasterDetector::StandaloneMasterDetector(const MasterInfo& leader)
            {
                process = new StandaloneMasterDetectorProcess(leader);
                spawn(process);
            }

            StandaloneMasterDetector::~StandaloneMasterDetector()
            {
                terminate(process);
                process::wait(process);
                delete process;
            }

            void StandaloneMasterDetector::appoint(const Option<MasterInfo>& leader)
            {
                dispatch(process, &StandaloneMasterDetectorProcess::appoint, leader);
            }

            process::Future<Option<MasterInfo>> StandaloneMasterDetector::detect(
                    const Option<mesos::MasterInfo> &previous) {
                return dispatch(process, &StandaloneMasterDetectorProcess::detect, previous);
            }
        }//namespace detector
    }//namespace master
}//namespace mesos

