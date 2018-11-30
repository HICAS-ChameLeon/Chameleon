

include_directories("/home/lilelr/open-source/libprocess-start/3rdparty/stout/include")
set(lele_env_tests_SRC env_tests.cpp)
add_executable(lele_env_tests_main ${lele_env_tests_SRC})
target_link_libraries(lele_env_tests_main  gmock gtest pthread)


set(lele_filesystem_tests_SRC filesystem_tests.cpp)
add_executable(lele_filesystem_tests_main ${lele_filesystem_tests_SRC})
target_link_libraries(lele_filesystem_tests_main  glog gmock gtest pthread)

set(lele_process_tests_SRC process_tests.cpp)
add_executable(lele_process_tests_main ${lele_process_tests_SRC})
target_link_libraries(lele_process_tests_main rt glog gmock gtest pthread)


set(lele_sendfile_tests_SRC sendfile_tests.cpp)
add_executable(lele_sendfile_tests_main ${lele_sendfile_tests_SRC})
target_link_libraries(lele_sendfile_tests_main  glog gmock gtest pthread)
