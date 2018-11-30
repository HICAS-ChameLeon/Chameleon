PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/messages monitor_info ${CMAKE_BINARY_DIR}/src/messages monitor_info)

set(slave_SRCS slave.hpp slave.cpp)
add_executable(slave ${slave_SRCS})
target_link_libraries(slave monitor_info hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})

set(Gpu_Collector_SRCS GpuCollector.cpp GpuCollector.hpp)
add_executable(yx_gpu_collector  ${Gpu_Collector_SRCS})
target_link_libraries(yx_gpu_collector hardware_resource ${PROCESS_LIBS}  ${PROCESS_TARGET})
