include_directories(${CMAKE_SOURCE_DIR}/src/messages)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages hardware_resource ${CMAKE_BINARY_DIR}/src/messages hardware_resource)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages runtime_resource ${CMAKE_BINARY_DIR}/src/messages runtime_resource)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages job ${CMAKE_BINARY_DIR}/src/messages job)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages cluster_operation ${CMAKE_BINARY_DIR}/src/messages cluster_operation)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages mesos ${CMAKE_BINARY_DIR}/src/messages mesos)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages scheduler ${CMAKE_BINARY_DIR}/src/messages scheduler)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages messages ${CMAKE_BINARY_DIR}/src/messages messages)
#PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages allocator ${CMAKE_BINARY_DIR}/src/messages allocator)





