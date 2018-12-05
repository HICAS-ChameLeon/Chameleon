PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages monitor_info ${CMAKE_BINARY_DIR}/src/messages monitor_info)

include_directories(${CMAKE_BINARY_DIR}/message)
include_directories(${CMAKE_CURRENT_LIST_DIR})

set(Gpu_Collector_SRCS gpu_collector.cpp gpu_collector.hpp runtime_resources_usage.hpp runtime_resources_usage.cpp)
add_executable(gpu_collector_tests  gpu_collector_tests.cpp ${Gpu_Collector_SRCS})
target_link_libraries(gpu_collector_tests hardware_resource  runtime_resource ${PROCESS_TARGET} gmock gtest)

set(disk_SRCS disk_collector.hpp disk_collector.cpp)
set(disk_colletor_test_SRCS disk_collector_tests.cpp ${disk_SRCS} runtime_resources_usage.hpp runtime_resources_usage.cpp)
add_executable(disk_colletor_test ${disk_colletor_test_SRCS})
target_link_libraries(disk_colletor_test hardware_resource runtime_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} gmock gtest)

set(cpu_SRCS cpu_collector.hpp cpu_collector.cpp)
set(cpu_collector_test_SRCS cpu_collector_tests.cpp ${cpu_SRCS})
add_executable(cpu_collector_tests ${cpu_collector_test_SRCS})
target_link_libraries(cpu_collector_tests hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} gmock gtest)


set(memory_SRCS memory_collector.hpp memory_collector.cpp)
set(memory_collector_test_SRCS memory_collector_tests.cpp ${memory_SRCS})
add_executable(memory_collector_tests ${memory_collector_test_SRCS}
        runtime_resources_usage.hpp runtime_resources_usage.cpp)
target_link_libraries(memory_collector_tests hardware_resource runtime_resource ${PROCESS_TARGET} gmock gtest)

set(tlb_SRCS tlb_collector.hpp tlb_collector.cpp)
set(tlb_collector_test_SRCS tlb_collector_tests.cpp ${tlb_SRCS})
add_executable(tlb_collector_tests ${tlb_collector_test_SRCS})
target_link_libraries(tlb_collector_tests hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} gmock gtest)

set(port_SRCS port_collector.cpp port_collector.hpp)
set(port_collector_tests_SRCS port_collector_tests.cpp ${port_SRCS})
add_executable(port_collector_tests ${port_collector_tests_SRCS})
target_link_libraries(port_collector_tests hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET} gmock gtest )


set(slave_SRCS slave.hpp slave.cpp resource_collector.hpp resource_collector.cpp ${cpu_SRCS} ${memory_SRCS} ${disk_SRCS}
        ${Gpu_Collector_SRCS} ${port_SRCS})

set(slave_SRCS slave.hpp slave.cpp resource_collector.hpp resource_collector.cpp ${cpu_SRCS} ${tlb_SRCS}
        ${memory_SRCS} ${disk_SRCS} ${Gpu_Collector_SRCS} )

add_executable(slave ${slave_SRCS})
target_link_libraries(slave monitor_info hardware_resource runtime_resource job ${PROCESS_LIBS}  ${PROCESS_TARGET})
