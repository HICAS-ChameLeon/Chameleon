
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages participant_info ${CMAKE_BINARY_DIR}/src/messages participant_info)

set(monitor_SRCS monitor.hpp monitor.cpp)
add_executable(monitor ${monitor_SRCS})
target_link_libraries(monitor participant_info ${PROCESS_LIBS}  ${PROCESS_TARGET})
