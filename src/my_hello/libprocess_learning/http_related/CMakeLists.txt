include_directories(${CMAKE_CURRENT_LIST_DIR})

add_executable(lele_encoder_tests encoder_tests.cpp encoder.hpp decoder.hpp )
target_link_libraries(lele_encoder_tests foo ${LELE_LIBS} gmock gtest pthread)

add_executable(lele_decoder_tests decoder_tests.cpp encoder.hpp decoder.hpp )
target_link_libraries(lele_decoder_tests foo ${LELE_LIBS} gmock gtest pthread)

#add_executable(lele_http_tests http_tests.cpp encoder.hpp decoder.hpp )
#target_link_libraries(lele_http_tests foo ${LELE_LIBS} gmock gtest pthread)