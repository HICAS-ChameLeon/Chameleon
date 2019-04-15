
set(CMAKE_CXX_STANDARD 11)
set(AliSimulator_SRCS AliTracePreProcessor.h
        AliTraceLoader.hpp AliTraceLoader.cpp
        AliSimulator.hpp AliSimulator.cpp
        SimulatorLinker.hpp SimulatorLinker.cpp
        SimulatedWallTime.hpp SimulatedWallTime.cpp
        AliSimulator_main.cpp)
add_executable(AliSimulator ${AliSimulator_SRCS})
target_link_libraries(AliSimulator glog mesos-1.3.2)