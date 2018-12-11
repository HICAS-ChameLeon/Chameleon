add_subdirectory(http_related)
add_subdirectory(resouce_limit)
include_directories(${CMAKE_CURRENT_LIST_DIR}/http_related)

add_executable(lele_after_tests after_tests.cpp)
target_link_libraries(lele_after_tests foo ${LELE_LIBS} gmock gtest pthread)

add_executable(lele_time_tests time_tests.cpp)
target_link_libraries(lele_time_tests foo ${LELE_LIBS} gmock gtest pthread)

add_executable(lele_future_tests future_tests.cpp)
target_link_libraries(lele_future_tests foo ${LELE_LIBS} gmock gtest pthread)

add_executable(lele_collect_tests collect_tests.cpp)
target_link_libraries(lele_collect_tests foo ${LELE_LIBS} gmock gtest pthread)

#add_executable(lele_io_tests io_tests.cpp)
#target_link_libraries(lele_io_tests foo ${LELE_LIBS} gtest gmock pthread)

add_executable(lele_mutex_tests mutex_tests.cpp)
target_link_libraries(lele_mutex_tests foo ${LELE_LIBS} gmock gtest pthread)

#add_executable(lele_process_tests process_tests.cpp)
#target_link_libraries(lele_process_tests foo ${LELE_LIBS} gmock gtest pthread)

add_executable(lele_subprocess_tests subprocess_tests.cpp)
target_link_libraries(lele_subprocess_tests foo ${LELE_LIBS} gmock gtest pthread)

add_executable(lele_limiter_tests limiter_tests.cpp)
target_link_libraries(lele_limiter_tests foo ${LELE_LIBS} gmock gtest pthread)


add_executable(lele_owned_tests owned_tests.cpp)
target_link_libraries(lele_owned_tests foo ${LELE_LIBS} gmock gtest pthread)

add_executable(lele_socket_tests socket_tests.cpp)
target_link_libraries(lele_socket_tests foo ${LELE_LIBS} gmock gtest pthread)