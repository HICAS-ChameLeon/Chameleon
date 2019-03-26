//
// Created by root on 19-3-14.
//

#ifndef CHAMELEON_CHAMELEON_PROTOBUF_UTILS_H
#define CHAMELEON_CHAMELEON_PROTOBUF_UTILS_H

// protobuf
#include <messages.pb.h>
#include <mesos.pb.h>
#include <initializer_list>
#include <ostream>
#include <set>
#include <string>

#include <process/time.hpp>

#include <stout/duration.hpp>
#include <stout/ip.hpp>
#include <stout/none.hpp>
#include <stout/option.hpp>
#include <stout/uuid.hpp>



namespace chameleon {
    namespace protobuf {
        // Helper function that creates a new task status from scratch with
// obligatory fields set.
        inline mesos::TaskStatus createTaskStatus(
                const mesos::TaskID &taskId,
                const mesos::TaskState &state,
                const UUID &uuid,
                double timestamp) {
            mesos::TaskStatus status;

            status.set_uuid(uuid.toBytes());
            status.set_timestamp(timestamp);
            status.mutable_task_id()->CopyFrom(taskId);
            status.set_state(state);

            return status;
        }
    }
}
#endif //CHAMELEON_CHAMELEON_PROTOBUF_UTILS_H
