
include_directories(${CMAKE_SOURCE_DIR}/src)

set(sched_SRC
        sched.cpp
        ${CMAKE_SOURCE_DIR}/src/common/parse.hpp
        ${CMAKE_SOURCE_DIR}/src/docker/type_utils.hpp
        ${CMAKE_SOURCE_DIR}/src/docker/type_utils.cpp
        ${CMAKE_SOURCE_DIR}/src/docker/attributes.hpp
        ${CMAKE_SOURCE_DIR}/src/docker/attributes.cpp
        ${CMAKE_SOURCE_DIR}/src/docker/values.hpp
        ${CMAKE_SOURCE_DIR}/src/docker/values.cpp
        ${CMAKE_SOURCE_DIR}/src/docker/roles.hpp
        ${CMAKE_SOURCE_DIR}/src/docker/roles.cpp
        ${CMAKE_SOURCE_DIR}/src/docker/messages.hpp
        ${CMAKE_SOURCE_DIR}/src/docker/messages.cpp
        ${CMAKE_SOURCE_DIR}/src/docker/resources.hpp
        ${CMAKE_SOURCE_DIR}/src/docker/resources.cpp
        master/detector.hpp
        master/detector.cpp
        master/standalone.hpp
        master/standalone.cpp)

add_library(sched "SHARED" ${sched_SRC} )

target_link_libraries(sched ${PROCESS_LIBS}  ${PROCESS_TARGET})