#protobuf files directories
include_directories(${CMAKE_BINARY_DIR}/src/messages)

# the global configuration for glog
include_directories(${CMAKE_SOURCE_DIR}/src/log)
include_directories(${CMAKE_SOURCE_DIR}/src/common)

# libprocess libraries headers directories
include_directories(${PROCESS_LIB_DIRS})

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages hardware_resource ${CMAKE_BINARY_DIR}/src/messages hardware_resource)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages runtime_resource ${CMAKE_BINARY_DIR}/src/messages runtime_resource)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages mesos ${CMAKE_BINARY_DIR}/src/messages mesos)

# libprocess .so .a libraries directories
link_directories(${PROCESS_LIB_DIRS})

add_subdirectory(common)

add_subdirectory(messages)

add_subdirectory(log)

add_subdirectory(master)

add_subdirectory(slave)

add_subdirectory(submitter)

add_subdirectory(launcher)

#add_subdirectory(my_hello)