include(ExternalProject)

### protobuf_2_6
set(protobuf_2_6_URL       https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz)

set(protobuf_2_6_ROOT ${CMAKE_BINARY_DIR}/3rdparty/protobuf_2_6)
set(protobuf_2_6_LIB_DIR ${protobuf_2_6_ROOT}/lib)
set(protobuf_2_6_INCLUDE_DIR ${protobuf_2_6_ROOT}/include)
#message(9 ${protobuf_2_6_ROOT})

#set(glag_configure ${protobuf_2_6_ROOT}/src/protobuf_2_6/configure --prefix=${protobuf_2_6_ROOT})
set(protobuf_2_6_configure cd ${protobuf_2_6_ROOT}/src/protobuf_2_6 && ./autogen.sh && ./configure --prefix=${protobuf_2_6_ROOT})

set(protobuf_2_6_make cd ${protobuf_2_6_ROOT}/src/protobuf_2_6 && make)
set(protobuf_2_6_install cd ${protobuf_2_6_ROOT}/src/protobuf_2_6 && make install)
set(protobuf_2_6_compiler ${protobuf_2_6_ROOT}/bin/protoc) # 定义protobuf 　protoc 编译器路径，方便　protobuf_compile.cmake 中函数　PROTOC_COMPILE使用

set(protobuf_2_6_TARGET ${protobuf_2_6_LIB_DIR}/libprotobuf.so)
#https://cmake.org/cmake/help/v3.0/module/ExternalProject.html
ExternalProject_Add(protobuf_2_6
        URL ${protobuf_2_6_URL} # Full path or URL of source
        DOWNLOAD_NAME protobuf-2.6.1.tar.gz # File name to store (if not end of URL)
        PREFIX ${protobuf_2_6_ROOT} # Root dir for entire project
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        CONFIGURE_COMMAND ${protobuf_2_6_configure}  # Build tree configuration command
        BUILD_COMMAND ${protobuf_2_6_make} # Command to drive the native build
        INSTALL_COMMAND ${protobuf_2_6_install} # Command to drive install after build
        #        [INSTALL_DIR dir]           # Installation prefix
        )