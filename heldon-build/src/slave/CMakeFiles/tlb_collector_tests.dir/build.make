# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/lemaker/software/clion-2018.2.5/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/lemaker/software/clion-2018.2.5/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lemaker/open-source/Chameleon

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lemaker/open-source/Chameleon/heldon-build

# Include any dependencies generated for this target.
include src/slave/CMakeFiles/tlb_collector_tests.dir/depend.make

# Include the progress variables for this target.
include src/slave/CMakeFiles/tlb_collector_tests.dir/progress.make

# Include the compile flags for this target's objects.
include src/slave/CMakeFiles/tlb_collector_tests.dir/flags.make

src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.o: src/slave/CMakeFiles/tlb_collector_tests.dir/flags.make
src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.o: ../src/slave/resource_collector/tlb_collector_tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.o"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.o -c /home/lemaker/open-source/Chameleon/src/slave/resource_collector/tlb_collector_tests.cpp

src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.i"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lemaker/open-source/Chameleon/src/slave/resource_collector/tlb_collector_tests.cpp > CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.i

src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.s"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lemaker/open-source/Chameleon/src/slave/resource_collector/tlb_collector_tests.cpp -o CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.s

src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.o: src/slave/CMakeFiles/tlb_collector_tests.dir/flags.make
src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.o: ../src/slave/resource_collector/tlb_collector.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.o"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.o -c /home/lemaker/open-source/Chameleon/src/slave/resource_collector/tlb_collector.cpp

src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.i"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lemaker/open-source/Chameleon/src/slave/resource_collector/tlb_collector.cpp > CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.i

src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.s"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lemaker/open-source/Chameleon/src/slave/resource_collector/tlb_collector.cpp -o CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.s

# Object files for target tlb_collector_tests
tlb_collector_tests_OBJECTS = \
"CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.o" \
"CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.o"

# External object files for target tlb_collector_tests
tlb_collector_tests_EXTERNAL_OBJECTS =

src/slave/tlb_collector_tests: src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector_tests.cpp.o
src/slave/tlb_collector_tests: src/slave/CMakeFiles/tlb_collector_tests.dir/resource_collector/tlb_collector.cpp.o
src/slave/tlb_collector_tests: src/slave/CMakeFiles/tlb_collector_tests.dir/build.make
src/slave/tlb_collector_tests: src/libhardware_resource.a
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libapr-1.so
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libz.so
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libapr-1.so
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libz.so
src/slave/tlb_collector_tests: 3rdparty/libprocess/src/libprocess-0.0.1.so.0.0.1
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libapr-1.so
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libz.so
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libapr-1.so
src/slave/tlb_collector_tests: /usr/lib/x86_64-linux-gnu/libz.so
src/slave/tlb_collector_tests: src/slave/CMakeFiles/tlb_collector_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable tlb_collector_tests"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tlb_collector_tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/slave/CMakeFiles/tlb_collector_tests.dir/build: src/slave/tlb_collector_tests

.PHONY : src/slave/CMakeFiles/tlb_collector_tests.dir/build

src/slave/CMakeFiles/tlb_collector_tests.dir/clean:
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/slave && $(CMAKE_COMMAND) -P CMakeFiles/tlb_collector_tests.dir/cmake_clean.cmake
.PHONY : src/slave/CMakeFiles/tlb_collector_tests.dir/clean

src/slave/CMakeFiles/tlb_collector_tests.dir/depend:
	cd /home/lemaker/open-source/Chameleon/heldon-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lemaker/open-source/Chameleon /home/lemaker/open-source/Chameleon/src/slave /home/lemaker/open-source/Chameleon/heldon-build /home/lemaker/open-source/Chameleon/heldon-build/src/slave /home/lemaker/open-source/Chameleon/heldon-build/src/slave/CMakeFiles/tlb_collector_tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/slave/CMakeFiles/tlb_collector_tests.dir/depend

