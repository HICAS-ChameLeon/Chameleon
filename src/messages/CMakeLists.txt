include_directories(${CMAKE_SOURCE_DIR}/src/messages)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages job ${CMAKE_BINARY_DIR}/src/messages job)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages cluster_operation ${CMAKE_BINARY_DIR}/src/messages cluster_operation)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages scheduler ${CMAKE_BINARY_DIR}/src/messages scheduler)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages messages ${CMAKE_BINARY_DIR}/src/messages messages)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages super_master_related ${CMAKE_BINARY_DIR}/src/messages super_master_related)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages slave_related ${CMAKE_BINARY_DIR}/src/messages slave_related)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages quota ${CMAKE_BINARY_DIR}/src/messages quota)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages master_p ${CMAKE_BINARY_DIR}/src/messages master_p)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages module ${CMAKE_BINARY_DIR}/src/messages module)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages maintenance ${CMAKE_BINARY_DIR}/src/messages maintenance)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages containerizer ${CMAKE_BINARY_DIR}/src/messages containerizer)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages spec ${CMAKE_BINARY_DIR}/src/messages spec)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages v1 ${CMAKE_BINARY_DIR}/src/messages v1)







