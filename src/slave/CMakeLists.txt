
add_subdirectory(software_store)
include_directories(resource_collector)


# set the gtest 1.7 relevant library names.
set(GTEST_LIBS gmock gtest gflags)

include_directories(${CMAKE_CURRENT_LIST_DIR})

#Gpu
set(Gpu_Collector_SRCS
        resource_collector/gpu_collector.cpp
        resource_collector/gpu_collector.hpp
        resource_collector/runtime_resources_usage.hpp
        resource_collector/runtime_resources_usage.cpp)
add_executable(gpu_collector_tests
        resource_collector/gpu_collector_tests.cpp
        ${Gpu_Collector_SRCS})
target_link_libraries(gpu_collector_tests
        hardware_resource
        runtime_resource
        ${PROCESS_TARGET}
        ${GTEST_LIBS})

#Disk
set(disk_SRCS
        resource_collector/disk_collector.hpp
        resource_collector/disk_collector.cpp)
set(disk_colletor_test_SRCS
        resource_collector/disk_collector_tests.cpp
        resource_collector/runtime_resources_usage.hpp
        resource_collector/runtime_resources_usage.cpp
        ${disk_SRCS})
add_executable(disk_colletor_test
        ${disk_colletor_test_SRCS})
target_link_libraries(disk_colletor_test
        hardware_resource
        runtime_resource
        ${PROCESS_LIBS}
        ${PROCESS_TARGET}
        ${GTEST_LIBS})

#Cpu
set(cpu_SRCS
        resource_collector/cpu_collector.hpp
        resource_collector/arm_cpu_collector.cpp
        resource_collector/x86_cpu_collector.cpp
        resource_collector/cpu_collector.cpp)
set(cpu_collector_test_SRCS
        resource_collector/cpu_collector_tests.cpp
        ${cpu_SRCS})
add_executable(cpu_collector_tests
        ${cpu_collector_test_SRCS})
target_link_libraries(cpu_collector_tests
        hardware_resource
        ${PROCESS_LIBS}
        ${PROCESS_TARGET}
        ${GTEST_LIBS})

#TLB
set(tlb_SRCS
        resource_collector/tlb_collector.hpp
        resource_collector/tlb_collector.cpp)
set(tlb_collector_test_SRCS
        resource_collector/tlb_collector_tests.cpp
        ${tlb_SRCS})
add_executable(tlb_collector_tests
        ${tlb_collector_test_SRCS})
target_link_libraries(tlb_collector_tests
        hardware_resource
        ${PROCESS_LIBS}
        ${PROCESS_TARGET}
        ${GTEST_LIBS})

#Memory
set(memory_SRCS
        resource_collector/memory_collector.hpp
        resource_collector/memory_collector.cpp)
set(memory_collector_test_SRCS
        resource_collector/memory_collector_tests.cpp
        ${memory_SRCS})
add_executable(memory_collector_tests
        resource_collector/runtime_resources_usage.hpp
        resource_collector/runtime_resources_usage.cpp
        ${memory_collector_test_SRCS})
target_link_libraries(memory_collector_tests
        hardware_resource
        runtime_resource
        ${PROCESS_TARGET}
        ${GTEST_LIBS})


#Port
set(port_SRCS
        resource_collector/port_collector.cpp
        resource_collector/port_collector.hpp
        resource_collector/runtime_resources_usage.hpp
        resource_collector/runtime_resources_usage.cpp)
set(port_collector_tests_SRCS
        resource_collector/port_collector_tests.cpp
        ${port_SRCS})
add_executable(port_collector_tests
        ${port_collector_tests_SRCS})
target_link_libraries(port_collector_tests
        hardware_resource
        runtime_resource
        ${PROCESS_LIBS}
        ${PROCESS_TARGET}
        ${GTEST_LIBS} )


set(runtime_resource_SRCS
        resource_collector/runtime_resources_usage.hpp
        resource_collector/runtime_resources_usage.cpp)

set(slave_SRCS
        slave.hpp
        slave.cpp
        resource_collector/resource_collector.hpp
        resource_collector/resource_collector.cpp)

# software_store
set(software_resources_SRCS
        software_store/software_resource_manager.hpp
        software_store/software_resource_manager.cpp)

list(APPEND slave_SRCS ${runtime_resource_SRCS})
list(APPEND slave_SRCS ${cpu_SRCS})
list(APPEND slave_SRCS ${memory_SRCS})
list(APPEND slave_SRCS ${disk_SRCS})
list(APPEND slave_SRCS ${Gpu_Collector_SRCS})
list(APPEND slave_SRCS ${tlb_SRCS})
list(APPEND slave_SRCS ${port_SRCS})
list(APPEND slave_SRCS ${software_resources_SRCS})
list(REMOVE_DUPLICATES slave_SRCS)

add_executable(slave ${slave_SRCS})

set(slave_PROTOS monitor_info)
list(APPEND slave_PROTOS hardware_resource)
list(APPEND slave_PROTOS runtime_resource)
list(APPEND slave_PROTOS job)
list(APPEND slave_PROTOS cluster_operation)
list(APPEND slave_PROTOS messages)
list(APPEND slave_PROTOS scheduler)
list(APPEND slave_PROTOS mesos)
list(APPEND slave_PROTOS slave_related)
list(APPEND slave_PROTOS fetcher)

list(REMOVE_DUPLICATES slave_PROTOS)
target_link_libraries(slave ${slave_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET} ${GTEST_LIBS})

