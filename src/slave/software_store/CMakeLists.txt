set(DOWNLODER_SRCS downloader.hpp downloader.cpp hdfs.hpp hdfs.cpp)
add_executable(downloader ${DOWNLODER_SRCS})
target_link_libraries(downloader ${PROCESS_LIBS}  ${PROCESS_TARGET} glog curl)