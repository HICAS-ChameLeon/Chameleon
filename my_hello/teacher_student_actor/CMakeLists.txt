# teacher_protobuf
# 该函数定义在　cmake/porotobuf_compile.cmake 中，　其中teacher　是将生成.cc 和　.h 包装成静态库，即　libteacher.a
PROTOC_COMPILE(${CMAKE_SOURCE_DIR}/my_hello/proto Teacher  ${CMAKE_BINARY_DIR}/my_hello/proto teacher)

include_directories(${CMAKE_BINARY_DIR}/my_hello/proto)
set(teacher_protobuf_SRC teacher_protobuf.cpp)
add_executable(lele_teacher_protobuf ${teacher_protobuf_SRC})
target_link_libraries(lele_teacher_protobuf teacher protobuf ${LELE_LIBS})

# student_protobuf
set(student_protobuf_SRC student_protobuf.cpp)
add_executable(lele_student_protobuf ${student_protobuf_SRC})
target_link_libraries(lele_student_protobuf teacher protobuf ${LELE_LIBS})