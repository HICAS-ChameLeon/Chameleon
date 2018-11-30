PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages monitor_info ${CMAKE_BINARY_DIR}/src/messages monitor_info)

set(slave_SRCS slave.hpp slave.cpp)
add_executable(slave ${slave_SRCS})
target_link_libraries(slave monitor_info hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

include_directories(${CMAKE_CURRENT_LIST_DIR})
set(disk_SRCS disk_collector.hpp disk_collector_tests.cpp)
add_executable(disk ${disk_SRCS})
target_link_libraries(disk hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} gmock gtest)
