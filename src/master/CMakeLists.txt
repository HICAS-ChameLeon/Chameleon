#PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages participant_info ${CMAKE_BINARY_DIR}/src/messages participant_info)

include_directories(${CMAKE_SOURCE_DIR}/src/slave/resource_collector)
include_directories(${CMAKE_SOURCE_DIR}/src/master/)
message(${CMAKE_SOURCE_DIR}/src/slave/resource_collector)

# set the gtest 1.7 relevant library names.
set(GTEST_LIBS gmock gtest gflags)

set(master_SRCS
        master.hpp
        master.cpp
        slave_object.hpp
        slave_object.cpp
        scheduler_interface.hpp
        coarse_grained_scheduler.hpp
        smhc_grained_scheduler.hpp)
set(master_PROTOS job)
list(APPEND master_PROTOS hardware_resource)
list(APPEND master_PROTOS runtime_resource)
list(APPEND master_PROTOS cluster_operation)
list(APPEND master_PROTOS scheduler)
list(APPEND master_PROTOS mesos)
list(APPEND master_PROTOS messages)
list(APPEND master_PROTOS super_master_related)
list(APPEND master_PROTOS slave_related)
list(REMOVE_DUPLICATES master_PROTOS)

add_executable(master ${master_SRCS})
target_link_libraries(master gflags ${master_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET})

#link_libraries(/home/marcie/chameleon/mesos-1.3.2/build/src/.libs/libmesos.so)
set(super_master_PROTOS super_master_related)
list(APPEND super_master_PROTOS hardware_resource)
list(APPEND super_master_PROTOS runtime_resource)
list(APPEND super_master_PROTOS messages)
set(super_master_SRCS
        super_master.hpp
        super_master.cpp
        slave_object.hpp
        slave_object.cpp)
add_executable(super_master ${super_master_SRCS})
target_link_libraries(super_master gflags ${super_master_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET} ${master_PROTOS})

