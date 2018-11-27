PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages monitor_info ${CMAKE_BINARY_DIR}/src/messages monitor_info)

set(participant_SRCS participant.hpp participant.cpp)
add_executable(participant ${participant_SRCS})
target_link_libraries(participant monitor_info ${PROCESS_LIBS}  ${PROCESS_TARGET})