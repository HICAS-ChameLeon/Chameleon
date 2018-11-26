
message(2 ${PROCESS_LIB_DIRS})

include_directories(${PROCESS_LIB_DIRS})
message(4 ${PROCESS_LIBS})
message(5 ${PROCESS_TARGET})
message(7 ${CMAKE_SOURCE_DIR})

PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages participant_info ${CMAKE_BINARY_DIR}/src/messages participant_info)

link_directories(${PROCESS_LIB_DIRS})

set(monitor_SRCS monitor.hpp monitor.cpp)
add_executable(monitor ${monitor_SRCS})
#target_link_libraries(monitor ${PROCESS_LIBS} ${PROCESS_TARGET})
target_link_libraries(monitor participant_info ${PROCESS_LIBS}  ${PROCESS_TARGET})
