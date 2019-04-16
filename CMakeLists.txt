cmake_minimum_required(VERSION 2.8.10)
SET(CMAKE_SYSTEM_NAME Linux)
#SET(CMAKE_C_COMPILER “/opt/riscv/bin/riscv64-unknown-elf-gcc”)
#SET(CMAKE_CXX_COMPILER “/opt/riscv/bin/riscv64-unknown-elf-gcc”)
#SET(CMAKE_FIND_ROOT_PATH “/opt/riscv”)


ADD_DEFINITIONS(
        -std=c++11 # Or -std=c++0x
)

project(Chameleon)

#message(${CMAKE_BINARY_DIR})
#message(${CMAKE_SOURCE_DIR})
#message(${CMAKE_CURRENT_LIST_DIR})

#execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0
#                   COMMAND  ./my_make.sh
#        )

### for risc-v crossing compiling
include_directories("/opt/riscv-toolchain/riscv64-unknown-linux-gnu/include")
link_directories("/opt/riscv-toolchain/riscv64-unknown-linux-gnu/lib")
include_directories("/home/sdc/4/mesos-1.3.2/riscv64_libraries/include")
link_directories("/home/sdc/4/mesos-1.3.2/riscv64_libraries/lib")
include_directories("/nfsroot/rv/hpc/riscv_chameleon/include")
link_directories("/nfsroot/rv/hpc/riscv_chameleon/lib")




### end risc-v

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

include_directories(${CMAKE_BINARY_DIR}/3rdparty/boost-1.53.0-prefix/src/boost-1.53.0)

include(cmake/protobuf_download.cmake)
include(cmake/protobuf_compile.cmake)
include(cmake/gflag.cmake)

include_directories(${CMAKE_BINARY_DIR}/3rdparty/protobuf_2_6/include)
# protobuf download and function

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/picojson-1.3.0)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/stout/include)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/libprocess/include)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/lib/.libs)

include_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/gtest/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/gtest/lib/.libs)
### for gmock-1.7.0 cmake
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/gmock-1.7.0/gtest/)


include_directories(${PROJECT_SOURCE_DIR}/3rdparty/gflag-2.2.2/include)
link_directories(${PROJECT_SOURCE_DIR}/3rdparty/gflag-2.2.2/lib)

include_directories(${CMAKE_BINARY_DIR}/3rdparty/gflag-2.2.2/include)
link_directories(${CMAKE_BINARY_DIR}/3rdparty/gflag-2.2.2/lib)

include_directories(${CMAKE_BINARY_DIR}/3rdparty/glog-0.3.3/src/glog-0.3.3-build/src/glog)
include_directories(${CMAKE_BINARY_DIR}/3rdparty/glog-0.3.3/src/glog-0.3.3-build/src)
include_directories(${CMAKE_BINARY_DIR}/3rdparty/glog-0.3.3/src/glog-0.3.3/src)
include_directories(${CMAKE_BINARY_DIR}/3rdparty/glog-0.3.3/src/windows)
link_directories(${CMAKE_BINARY_DIR}/3rdparty/glog-0.3.3/src/glog-0.3.3-build/.libs)




add_subdirectory(3rdparty)
#add_subdirectory(src/my_hello)

add_subdirectory(src)


