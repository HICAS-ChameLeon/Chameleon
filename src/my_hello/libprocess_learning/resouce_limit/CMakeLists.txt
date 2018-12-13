
#limit bilko
add_executable(lele_disk_io_of_a_process limit_disk_io_limit_of_a_process.cpp)
target_link_libraries(lele_disk_io_of_a_process  ${LELE_LIBS} gmock gtest pthread)

#limit cpu
add_executable(lele_limit_cpu limit_cpu.cpp)
target_link_libraries(lele_limit_cpu ${LELE_LIBS} gmock gtest pthread)

#weiguo limit memory
add_executable(weiguo_limit_memory weiguo_limit_memory.cpp)
target_link_libraries(weiguo_limit_memory ${LELE_LIBS} pthread)

#weiguo limit cpu
add_executable(weiguo_limit_cpu weiguo_limit_cpu.cpp)
target_link_libraries(weiguo_limit_cpu ${LELE_LIBS} pthread)