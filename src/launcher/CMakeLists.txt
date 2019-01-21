include_directories(${CMAKE_CURRENT_LIST_DIR})

set(executor_SRCS executor.cpp executor.hpp)
set(exec_process_SRCS exec.cpp exec.hpp)

add_executable(chameleon-executor  ${executor_SRCS} ${exec_process_SRCS})

set(executor_PROTOS mesos)
list(APPEND executor_PROTOS messages)
list(REMOVE_DUPLICATES executor_PROTOS)

target_link_libraries(chameleon-executor ${executor_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET} ${GTEST_LIBS})