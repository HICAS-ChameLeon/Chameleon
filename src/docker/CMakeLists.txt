


set(DOCKER_SRC docker.hpp docker.cpp)
set(DOCKER_EXECUTOR_SRC executor.hpp executor.cpp)


add_executable(docker ${DOCKER_SRC})
target_link_libraries(docker ${PROCESS_LIBS}  ${PROCESS_TARGET} mesos)

#add_executable(docker_executor ${DOCKER_EXECUTOR_SRC})
#target_link_libraries(docker_executor ${PROCESS_LIBS}  ${PROCESS_TARGET})

#include_directories(/usr/local/include)
add_executable(docker_test docker_test.cpp type_utils.cpp values.cpp roles.cpp attributes.cpp)
target_link_libraries(docker_test ${PROCESS_LIBS} ${PROCESS_TARGET} mesos)