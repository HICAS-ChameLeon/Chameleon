
include_directories(${CMAKE_SOURCE_DIR}/src)

set(sched_SRC
        constants.hpp
        flags.hpp
        sched.cpp
        ${CMAKE_SOURCE_DIR}/src/common/parse.hpp
        ${CMAKE_SOURCE_DIR}/src/logging/logging.hpp
        ${CMAKE_SOURCE_DIR}/src/logging/logging.cpp
        ${CMAKE_SOURCE_DIR}/src/logging/flags.hpp
        ${CMAKE_SOURCE_DIR}/src/logging/flags.cpp)

add_library(sched "SHARED" ${sched_SRC} )