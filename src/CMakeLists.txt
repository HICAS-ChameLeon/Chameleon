#protobuf files directories
include_directories(${CMAKE_BINARY_DIR}/src/messages)

# the global configuration for glog
include_directories(${CMAKE_SOURCE_DIR}/src/log)
include_directories(${CMAKE_SOURCE_DIR}/src/common)
include_directories(${CMAKE_SOURCE_DIR}/src/slave/resource_collector)
include_directories(${CMAKE_SOURCE_DIR}/src/docker)
include_directories(${CMAKE_SOURCE_DIR}/src/logging)
include_directories(${CMAKE_SOURCE_DIR}/src/linux)
include_directories(${CMAKE_SOURCE_DIR}/src/sched)




# libprocess libraries headers directories
include_directories(${PROCESS_LIB_DIRS})

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages mesos ${CMAKE_BINARY_DIR}/src/messages mesos)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages fetcher ${CMAKE_BINARY_DIR}/src/messages fetcher)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages hardware_resource ${CMAKE_BINARY_DIR}/src/messages hardware_resource)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages runtime_resource ${CMAKE_BINARY_DIR}/src/messages runtime_resource)


# libprocess .so .a libraries directories
link_directories(${PROCESS_LIB_DIRS})

add_subdirectory(common)

add_subdirectory(messages)

add_subdirectory(log)

add_subdirectory(master)

add_subdirectory(slave)

add_subdirectory(submitter)

add_subdirectory(launcher)
add_subdirectory(docker)
add_subdirectory(test)
add_subdirectory(simulator)
#set(sched_SRC
#        sched/constants.hpp
#        sched/flags.hpp
#        sched/sched.cpp)
#
#add_library(sched "STATIC" ${sched_SRC})