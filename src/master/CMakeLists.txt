PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages participant_info ${CMAKE_BINARY_DIR}/src/messages participant_info)

# set the gtest 1.7 relevant library names.
set(GTEST_LIBS gmock gtest gflags)

set(master_SRCS  master.hpp master.cpp)
set(master_PROTOS participant_info)
list(APPEND master_PROTOS job)
list(APPEND master_PROTOS hardware_resource)
list(APPEND master_PROTOS runtime_resource)
list(APPEND master_PROTOS cluster_operation)
list(REMOVE_DUPLICATES master_PROTOS)

add_executable(master ${master_SRCS})
target_link_libraries(master gflags ${master_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET})
