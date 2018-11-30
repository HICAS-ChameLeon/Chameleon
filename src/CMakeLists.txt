#protobuf files directories
include_directories(${CMAKE_BINARY_DIR}/src/messages)

# libprocess libraries headers directories
include_directories(${PROCESS_LIB_DIRS})

# libprocess .so .a libraries directories
link_directories(${PROCESS_LIB_DIRS})

add_subdirectory(messages)

add_subdirectory(master)

add_subdirectory(slave)

#add_subdirectory(my_hello)