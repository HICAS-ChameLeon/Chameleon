//
// Created by root on 19-5-6.
//

#ifndef CHAMELEON_STANDALONE_HPP
#define CHAMELEON_STANDALONE_HPP

#include <string>

#include <process/future.hpp>

#include <stout/option.hpp>

#include <mesos.pb.h>

#include "detector.hpp"

namespace mesos{
    namespace master{
        namespace detector{

            //Forward declaration
            class StandaloneMasterDetectorProcess;

            class StandaloneMasterDetector : public MasterDetector{
            public:
                StandaloneMasterDetector();

                explicit StandaloneMasterDetector(const MasterInfo& leader);

                virtual ~StandaloneMasterDetector();

                // Appoint the leading master so it can be *detected*.
                void appoint(const Option<MasterInfo>& leader);

                virtual process::Future<Option<MasterInfo>> detect(const Option<MasterInfo>& previous = None());
            private:
                StandaloneMasterDetectorProcess* process;
            };

        }//namespace detector
    }//namespace master
}//namespace mesos

#endif //CHAMELEON_STANDALONE_HPP
