#protobuf files directories
include_directories(${CMAKE_BINARY_DIR}/src/messages)

# the global configuration for glog
message(5 ${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/log)

# libprocess libraries headers directories
include_directories(${PROCESS_LIB_DIRS})

# libprocess .so .a libraries directories
link_directories(${PROCESS_LIB_DIRS})

add_subdirectory(messages)

add_subdirectory(log)

add_subdirectory(master)

add_subdirectory(slave)

#add_subdirectory(my_hello)