### downloader executable
set(DOWNLODER_SRCS downloader.hpp downloader.cpp hdfs.hpp hdfs.cpp)
set(downloader_PROTOS fetcher)

add_executable(downloader ${DOWNLODER_SRCS})
target_link_libraries(downloader ${downloader_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET} glog curl)

### software_resource_manager
set(Software_resources_manager_SRCS software_resource_manager.hpp software_resource_manager.cpp)
set(manager_PROTOS fetcher)
add_executable(software_resource_manager ${Software_resources_manager_SRCS})
target_link_libraries(software_resource_manager ${manager_PROTOS} ${PROCESS_LIBS}  ${PROCESS_TARGET})