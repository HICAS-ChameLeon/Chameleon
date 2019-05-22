message(STATUS "downloading...
     src='https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz'
     dst='/home/heldon/chameleon/Chameleon/heldon-build/3rdparty/protobuf_2_6/src/protobuf-2.6.1.tar.gz'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz"
  "/home/heldon/chameleon/Chameleon/heldon-build/3rdparty/protobuf_2_6/src/protobuf-2.6.1.tar.gz"
  SHOW_PROGRESS
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
