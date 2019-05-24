include_directories(${CMAKE_SOURCE_DIR}/src/messages)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages job ${CMAKE_BINARY_DIR}/src/messages job)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages cluster_operation ${CMAKE_BINARY_DIR}/src/messages cluster_operation)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages scheduler ${CMAKE_BINARY_DIR}/src/messages scheduler)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages messages ${CMAKE_BINARY_DIR}/src/messages messages)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages super_master_related ${CMAKE_BINARY_DIR}/src/messages super_master_related)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages slave_related ${CMAKE_BINARY_DIR}/src/messages slave_related)





