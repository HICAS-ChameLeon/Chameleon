
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages participant_info ${CMAKE_BINARY_DIR}/src/messages participant_info)

set(master_SRCS master.hpp master.cpp)
add_executable(master ${master_SRCS})
target_link_libraries(master participant_info ${PROCESS_LIBS}  ${PROCESS_TARGET})
