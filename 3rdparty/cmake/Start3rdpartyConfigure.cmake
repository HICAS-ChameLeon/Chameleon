#message(${CMAKE_SOURCE_DIR})
# DEFINE DIRECTORY STRUCTURE FOR THIRD-PARTY LIBS.
##################################################
set(MESOS_3RDPARTY_SRC ${CMAKE_SOURCE_DIR}/3rdparty)
set(MESOS_3RDPARTY_BIN ${CMAKE_BINARY_DIR}/3rdparty)

if (NOT WIN32)
  # LevelDB does not build on Windows.
  EXTERNAL("leveldb" ${LEVELDB_VERSION} "${MESOS_3RDPARTY_BIN}")
endif (NOT WIN32)



# Convenience variables for include directories of third-party dependencies.
set(LEVELDB_INCLUDE_DIR ${LEVELDB_ROOT}/include)


# Convenience variables for "lflags", the symbols we pass to CMake to generate
# things like `-L/path/to/glog` or `-lglog`.
if (NOT WIN32)
  set(LEVELDB_LFLAG   ${LEVELDB_ROOT}/out-static/libleveldb.a)
#  set(ZOOKEEPER_LFLAG ${ZOOKEEPER_LIB}/lib/libzookeeper_mt.a)
else (NOT WIN32)
#  set(ZOOKEEPER_LFLAG zookeeper hashtable)
endif (NOT WIN32)

