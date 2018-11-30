PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages monitor_info ${CMAKE_BINARY_DIR}/src/messages monitor_info)

include_directories(${CMAKE_BINARY_DIR}/message)

set(slave_SRCS slave.hpp slave.cpp)

include_directories(${CMAKE_CURRENT_LIST_DIR})
set(slave_SRCS slave.hpp slave.cpp resource_collector.hpp resource_collector.cpp)

add_executable(slave ${slave_SRCS})
target_link_libraries(slave monitor_info hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

set(Gpu_Collector_SRCS gpu_collector.cpp gpu_collector.hpp)
add_executable(gpu_collector_test  ${Gpu_Collector_SRCS} gpu_collector_tests.cpp)
target_link_libraries(gpu_collector_test hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

include_directories(${CMAKE_CURRENT_LIST_DIR})
set(disk_SRCS disk_collector.hpp disk_collector_tests.cpp)
add_executable(disk ${disk_SRCS})

target_link_libraries(disk hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} gmock gtest)

set(cpu_SRCS cpu_collector_tests.cpp cpu_collector.hpp)
add_executable(cpu_collector ${cpu_SRCS})
target_link_libraries(cpu_collector hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

set(memory_SRCS memory_collector.hpp memory_collector_tests.cpp)
add_executable(memory ${memory_SRCS})
target_link_libraries(memory hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})