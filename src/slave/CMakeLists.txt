PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages monitor_info ${CMAKE_BINARY_DIR}/src/messages monitor_info)

include_directories(${CMAKE_BINARY_DIR}/message)
include_directories(${CMAKE_CURRENT_LIST_DIR})


# set the gtest 1.7 relevant library names.
set(GTEST_LIBS gmock gtest gflags)

set(Gpu_Collector_SRCS gpu_collector.cpp gpu_collector.hpp runtime_resources_usage.hpp runtime_resources_usage.cpp)
add_executable(gpu_collector_tests  gpu_collector_tests.cpp ${Gpu_Collector_SRCS})
target_link_libraries(gpu_collector_tests hardware_resource  runtime_resource ${PROCESS_TARGET} ${GTEST_LIBS})

set(disk_SRCS disk_collector.hpp disk_collector.cpp)
set(disk_colletor_test_SRCS disk_collector_tests.cpp ${disk_SRCS} runtime_resources_usage.hpp runtime_resources_usage.cpp)
add_executable(disk_colletor_test ${disk_colletor_test_SRCS})
target_link_libraries(disk_colletor_test hardware_resource runtime_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} ${GTEST_LIBS})

set(cpu_SRCS cpu_collector.hpp arm_cpu_collector.cpp x86_cpu_collector.cpp cpu_collector.cpp)
set(cpu_collector_test_SRCS cpu_collector_tests.cpp ${cpu_SRCS})
add_executable(cpu_collector_tests ${cpu_collector_test_SRCS})
target_link_libraries(cpu_collector_tests hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} ${GTEST_LIBS})

set(memory_SRCS memory_collector.hpp memory_collector.cpp)
set(memory_collector_test_SRCS memory_collector_tests.cpp ${memory_SRCS})
add_executable(memory_collector_tests ${memory_collector_test_SRCS}
        runtime_resources_usage.hpp runtime_resources_usage.cpp)
target_link_libraries(memory_collector_tests hardware_resource runtime_resource ${PROCESS_TARGET} ${GTEST_LIBS})

set(tlb_SRCS tlb_collector.hpp tlb_collector.cpp)
set(tlb_collector_test_SRCS tlb_collector_tests.cpp ${tlb_SRCS})
add_executable(tlb_collector_tests ${tlb_collector_test_SRCS})
target_link_libraries(tlb_collector_tests hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} ${GTEST_LIBS})

set(port_SRCS port_collector.cpp port_collector.hpp runtime_resources_usage.hpp runtime_resources_usage.cpp)
set(port_collector_tests_SRCS port_collector_tests.cpp ${port_SRCS})
add_executable(port_collector_tests ${port_collector_tests_SRCS})
target_link_libraries(port_collector_tests hardware_resource runtime_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} ${GTEST_LIBS} )

set(runtime_resource_SRCS runtime_resources_usage.hpp runtime_resources_usage.cpp)
set(slave_SRCS  slave.hpp slave.cpp  resource_collector.hpp resource_collector.cpp)

list(APPEND slave_SRCS ${runtime_resource_SRCS})
list(APPEND slave_SRCS ${cpu_SRCS})
list(APPEND slave_SRCS ${memory_SRCS})
list(APPEND slave_SRCS ${disk_SRCS})
list(APPEND slave_SRCS ${Gpu_Collector_SRCS})
list(APPEND slave_SRCS ${tlb_SRCS})
list(APPEND slave_SRCS ${port_SRCS})
list(REMOVE_DUPLICATES slave_SRCS)
#message(47 ${slave_SRCS})

add_executable(slave ${slave_SRCS})
set(slave_PROTOS monitor_info)
list(APPEND slave_PROTOS hardware_resource)
list(APPEND slave_PROTOS runtime_resource)
list(APPEND slave_PROTOS job)
list(APPEND slave_PROTOS cluster_operation)
list(APPEND slave_PROTOS messages)
list(APPEND slave_PROTOS scheduler)
list(APPEND slave_PROTOS mesos)

list(REMOVE_DUPLICATES slave_PROTOS)

target_link_libraries(slave ${slave_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET} ${GTEST_LIBS})

