add_subdirectory(os)
add_subdirectory(stout_test)

set(lele_stout_my_learning_SRC lele_stout_my_learning.cpp)
add_executable(lele_stout_my_learning_main ${lele_stout_my_learning_SRC})
target_link_libraries(lele_stout_my_learning_main  gmock gtest pthread)


set(lele_option_usage_SRC lele_option_usage.cpp)
add_executable(lele_option_usage_main ${lele_option_usage_SRC})
target_link_libraries(lele_option_usage_main  gmock gtest pthread)

set(lele_try_tests_SRC lele_try_tests.cpp)
add_executable(lele_try_tests_main ${lele_try_tests_SRC})
target_link_libraries(lele_try_tests_main  gmock gtest pthread)


set(lele_result_tests_SRC lele_result_tests.cpp)
add_executable(lele_result_tests_main ${lele_result_tests_SRC})
target_link_libraries(lele_result_tests_main  gmock gtest pthread)


set(lele_none_tests_SRC lele_none_tests.cpp)
add_executable(lele_none_tests_main ${lele_none_tests_SRC})
target_link_libraries(lele_none_tests_main  gmock gtest pthread)

set(lele_some_tests_SRC lele_some_tests.cpp)
add_executable(lele_some_tests_main ${lele_some_tests_SRC})
target_link_libraries(lele_some_tests_main  gmock gtest pthread)


set(lele_ip_tests_SRC ip_tests.cpp)
add_executable(lele_ip_tests_main ${lele_ip_tests_SRC})
target_link_libraries(lele_ip_tests_main  gmock gtest pthread)

set(lele_subcommand_SRC subcommand_tests.cpp)
add_executable(lele_subcommand_main ${lele_subcommand_SRC})
target_link_libraries(lele_subcommand_main  glog gmock gtest pthread)

set(lele_proc_tests_SRC proc_tests.cpp)
add_executable(lele_proc_tests_main ${lele_proc_tests_SRC})
target_link_libraries(lele_proc_tests_main  glog gmock gtest pthread)