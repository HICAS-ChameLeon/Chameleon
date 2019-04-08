include_directories(${CMAKE_SOURCE_DIR}/src/messages/to_protoc)

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages/to_protoc master_p ${CMAKE_BINARY_DIR}/src/messages master_p)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages/to_protoc containerizer ${CMAKE_BINARY_DIR}/src/messages containerizer)
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages/to_protoc v1 ${CMAKE_BINARY_DIR}/src/messages v1)