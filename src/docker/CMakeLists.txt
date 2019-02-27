
set(docker_SRC docker.hpp docker.cpp)
set(DOCKER_EXECUTOR_SRC executor.hpp executor.cpp)

#add_executable(docker_executor ${DOCKER_EXECUTOR_SRC})
#target_link_libraries(docker_executor ${PROCESS_LIBS}  ${PROCESS_TARGET})


set(type_utils_SRC type_utils.hpp type_utils.cpp)
set(attributes_SRC attributes.hpp attributes.cpp)
set(values_SRC values.hpp values.cpp)
set(roles_SRC roles.hpp roles.cpp)
set(messages_SRC messages.hpp messages.cpp)
set(resources_SRC resources.hpp resources.cpp)
set(docker_test_SRC docker_test.cpp ${type_utils_SRC}
        ${attributes_SRC} ${values_SRC} ${roles_SRC}
        ${messages_SRC} ${resources_SRC} ${docker_SRC})

add_executable(docker_test ${docker_test_SRC})
target_link_libraries(docker_test ${PROCESS_LIBS} ${PROCESS_TARGET} mesos messages)