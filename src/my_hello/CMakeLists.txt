add_subdirectory(proto)
add_subdirectory(stout_learning)

#message(${PROJECT_BINARY_DIR})
# Define third-party include directories. Tells compiler toolchain where to get
# headers for our third party libs (e.g., -I/path/to/glog on Linux).
###############################################################################
set(LELE_INCLUDE_DIRS
        ${LELE_INCLUDE_DIRS}
        ${MESOS_PUBLIC_INCLUDE_DIR}
        # Contains (e.g.) compiled *.pb.h files.
        ${PROJECT_BINARY_DIR}/proto/
        ${PROJECT_BINARY_DIR}/my_hello/proto/
        )
#message("71")
#message(${PROCESS_3RDPARTY_INCLUDE_DIRS})
set(LELE_3RDPARTY_INCLUDE_DIRS
        ${LELE_3RDPARTY_INCLUDE_DIRS}
        ${PROCESS_INCLUDE_DIRS}
        ${PROCESS_3RDPARTY_INCLUDE_DIRS}
        ${LEVELDB_INCLUDE_DIR}
        )

# Define third-party lib install directories. Used to tell the compiler
# toolchain where to find our third party libs (e.g., -L/path/to/glog on
# Linux).
########################################################################
set(LELE_LIB_DIRS
        ${PROCESS_LIB_DIRS}
        )

# Define third-party libs. Used to generate flags that the linker uses to
# include our third-party libs (e.g., -lglog on Linux).
#########################################################################
# 3rdparty/libprocess/cmake/ProcessConfigure.cmake 139 ${Process_libs}
# 49 ${PROCESS_TARGET}
set(LELE_LIBS
        ${PROCESS_LIBS}
        ${PROCESS_TARGET}
        )

# INCLUDE DIRECTIVES FOR LELE EXECUTABLE (generates, e.g., -I/path/to/thing
# on Linux).
############################################################################
include_directories(SYSTEM ${LELE_3RDPARTY_INCLUDE_DIRS})
include_directories(${LELE_INCLUDE_DIRS})

# LINKING LIBRARIES BY DIRECTORY (might generate, e.g., -L/path/to/thing on
# Linux).
###########################################################################
link_directories(${LELE_LIB_DIRS})

#use protobuf 2.6 to generate the relevant headers and cpps
# 该函数定义在　cmake/porotobuf_compile.cmake 中，　其中foo　是将生成.cc 和　.h 包装成静态库，即　libfoo.a
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/src/my_hello/proto Offer  ${CMAKE_BINARY_DIR}/src/my_hello/proto foo)

include_directories(${CMAKE_BINARY_DIR}/src/my_hello/proto)


# THE LELE EXECUTABLE.
#######################

set(LELE_HELLO "lele_server_protobuf")
set(lele_hello_SRC Lele_server_protobuf.cpp)
add_executable(${LELE_HELLO} ${lele_hello_SRC})

# ADD LINKER FLAGS (generates, e.g., -lglog on Linux).
######################################################
target_link_libraries(${LELE_HELLO} foo protobuf ${LELE_LIBS})

set(client_server_message_event_SRC client_server_message_event.cpp)
add_executable(client_server_message_event ${client_server_message_event_SRC})
target_link_libraries(client_server_message_event foo protobuf ${LELE_LIBS})

# client_protobuf
set(client_protobuf_SRC client_protobuf.cpp )
add_executable(client_protobuf ${client_protobuf_SRC})
target_link_libraries(client_protobuf foo protobuf ${LELE_LIBS})
#
## test_master
#set(test_master_SRC test-master.cpp)
#add_executable(test-master ${test_master_SRC})
#target_link_libraries(test-master foo protobuf ${LELE_LIBS})
#
## test_slave
#set(test-slave_SRC test-slave.cpp)
#add_executable(test-slave ${test-slave_SRC})
#target_link_libraries(test-slave foo protobuf ${LELE_LIBS})

# future promise
set(future_promise_SRC future_promise.cpp)
add_executable(future_promise ${future_promise_SRC})
target_link_libraries(future_promise foo protobuf ${LELE_LIBS} pthread gmock gtest)

#
set(http_process_learning_SRC http_process_learning.cpp)
add_executable(http_process_learning ${http_process_learning_SRC})
target_link_libraries(http_process_learning foo ${LELE_LIBS})


set(simpleserver_SRC simpleserver.cpp)
add_executable(simpleserver ${simpleserver_SRC})
target_link_libraries(simpleserver foo ${LELE_LIBS})


set(lele_http_simple_process_SRC lele_http_simple_process.cpp)
add_executable(lele_http_simple_process ${lele_http_simple_process_SRC})
target_link_libraries(lele_http_simple_process foo ${LELE_LIBS})

#message(${CMAKE_CURRENT_LIST_DIR})

set(hello_world_SRC hello_word.cpp)
add_executable(hello_world_main ${hello_world_SRC})


add_subdirectory(libprocess_learning)
add_subdirectory(teacher_student_actor)


