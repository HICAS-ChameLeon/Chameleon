PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages monitor_info ${CMAKE_BINARY_DIR}/src/messages monitor_info)

include_directories(${CMAKE_BINARY_DIR}/message)

set(slave_SRCS slave.hpp slave.cpp)

include_directories(${CMAKE_CURRENT_LIST_DIR})
set(slave_SRCS slave.hpp slave.cpp resource_collector.hpp resource_collector.cpp)

add_executable(slave ${slave_SRCS})
target_link_libraries(slave monitor_info hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

set(Gpu_Collector_SRCS GpuCollector.cpp GpuCollector.hpp)
add_executable(yx_gpu_collector  ${Gpu_Collector_SRCS})
target_link_libraries(yx_gpu_collector hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

include_directories(${CMAKE_CURRENT_LIST_DIR})
set(disk_SRCS disk_collector.hpp disk_collector.cpp)
add_executable(disk ${disk_SRCS})
target_link_libraries(disk hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

set(cpu_SRCS cpu_collector.cpp cpu_collector.hpp)
add_executable(cpu ${cpu_SRCS})
target_link_libraries(cpu hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

