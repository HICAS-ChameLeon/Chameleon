#set(Protobuf_INCLUDE_DIR "${CMAKE_BINARY_DIR}/3rdparty/protobuf-2.6.1/src/protobuf-2.6.1-lib/lib/include")
#find_package(Protobuf REQUIRED)
#
#if(PROTOBUF_FOUND)
#    message(STATUS "protobuf library found")
#    message(${PROTOBUF_INCLUDE_DIRS})
#else()
#    message(FATAL_ERROR "protobuf library is needed but cant be found")
#endif()
#
#include_directories(${PROTOBUF_INCLUDE_DIRS})
#message(${PROTOBUF_LIBRARIES})
#
#PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS Offer.proto addressbook.proto)
#
#
#message("lele my_hello")
#message(${PROTOBUF_INCLUDE_DIRS})
#PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS Offer.proto addressbook.proto Teacher.proto)

#add_library(foo ${PROTO_SRCS} ${PROTO_HDRS})


#target_include_directories(foo
#        PUBLIC
#        "${Protobuf_INCLUDE_DIRS}")
#target_link_libraries(foo ${PROTOBUF_LIBRARY} pthread)