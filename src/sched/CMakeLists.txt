
include_directories(${CMAKE_SOURCE_DIR}/src)

set(sched_SRC
        constants.hpp
        flags.hpp
        sched.cpp
        ${CMAKE_SOURCE_DIR}/src/common/parse.hpp)

add_library(sched "SHARED" ${sched_SRC})