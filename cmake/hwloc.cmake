include(ExternalProject)

### hwloc
set(hwloc_URL       https://download.open-mpi.org/release/hwloc/v1.11/hwloc-1.11.11.tar.gz)

set(hwloc_ROOT ${CMAKE_BINARY_DIR}/thirdparty/hwloc)
set(hwloc_LIB_DIR ${hwloc_ROOT}/lib)
set(hwloc_INCLUDE_DIR ${hwloc_ROOT}/include)
message(9 ${hwloc_ROOT})

#set(glag_configure ${hwloc_ROOT}/src/hwloc/configure --prefix=${hwloc_ROOT})
set(hwloc_configure cd ${hwloc_ROOT}/src/hwloc && ./configure --prefix=${hwloc_ROOT})

set(hwloc_make cd ${hwloc_ROOT}/src/hwloc && make)
set(hwloc_install cd ${hwloc_ROOT}/src/hwloc && make install)

#https://cmake.org/cmake/help/v3.0/module/ExternalProject.html
ExternalProject_Add(hwloc
        URL ${hwloc_URL} # Full path or URL of source
        DOWNLOAD_NAME hwloc-1.11.11.tar.gz # File name to store (if not end of URL)
        PREFIX ${hwloc_ROOT} # Root dir for entire project
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        CONFIGURE_COMMAND ${hwloc_configure}  # Build tree configuration command
        BUILD_COMMAND ${hwloc_make} # Command to drive the native build
        INSTALL_COMMAND ${hwloc_install} # Command to drive install after build
        #        [INSTALL_DIR dir]           # Installation prefix
        )