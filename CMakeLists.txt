cmake_minimum_required(VERSION 2.8.10)


project(Chameleon)

#message(${CMAKE_BINARY_DIR})
#message(${CMAKE_SOURCE_DIR})
#message(${CMAKE_CURRENT_LIST_DIR})

execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0
                   COMMAND  ./my_make.sh
        )

# CMAKE MODULE SETUP.f
#####################
# Paths that are searched when `include(...)` is called.
# 添加include 命令所搜索的路径
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/libprocess/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/stout/cmake)
list(
        APPEND
        CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/3rdparty/libprocess/cmake/macros)



# Macros.
# ${CMAKE_SOURCE_DIR}/cmake
include(Common)
# ${CMAKE_SOURCE_DIR}/3rdparty/libprocess/cmake/macros
include(External)
# ${CMAKE_SOURCE_DIR}/3rdparty/libprocess/cmake/macros
include(PatchCommand)
# ${CMAKE_SOURCE_DIR}/3rdparty/cmake
include(Versions)
# ${CMAKE_SOURCE_DIR}/3rdparty/cmake
include(cotire)

# Configuration.
# ${CMAKE_SOURCE_DIR}/cmake
include(StartConfigure)
# ${CMAKE_SOURCE_DIR}/cmake
include(CompilationConfigure)
# ${CMAKE_SOURCE_DIR}/3rdparty/libprocess/cmake
include(ProcessConfigure)


include(cmake/protobuf_download.cmake)
include(cmake/protobuf_compile.cmake)

include_directories(${CMAKE_BINARY_DIR}/3rdparty/protobuf_2_6/include)
# protobuf download and function

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/picojson-1.3.0)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/stout/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/libprocess/include)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/lib/.libs)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/gtest/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/gtest/lib/.libs)

include_directories(${CMAKE_BINARY_DIR}/3rdparty/glog-0.3.3/src/glog-0.3.3-build/src)
link_directories(${CMAKE_BINARY_DIR}/3rdparty/glog-0.3.3/src/glog-0.3.3-build/.libs)

add_subdirectory(3rdparty)
add_subdirectory(src/my_hello)


