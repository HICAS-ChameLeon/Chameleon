
set(submitter_SRCS submitter.hpp submitter.cpp)

add_executable(submitter ${submitter_SRCS})
target_link_libraries(submitter gflags job ${PROCESS_LIBS}  ${PROCESS_TARGET})