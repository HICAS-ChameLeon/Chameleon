# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lemaker/open-source/Chameleon

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lemaker/open-source/Chameleon/heldon-build

# Include any dependencies generated for this target.
include src/messages/CMakeFiles/scheduler.dir/depend.make

# Include the progress variables for this target.
include src/messages/CMakeFiles/scheduler.dir/progress.make

# Include the compile flags for this target's objects.
include src/messages/CMakeFiles/scheduler.dir/flags.make

src/messages/scheduler.pb.cc:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating scheduler.pb.cc, scheduler.pb.h"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && ../../3rdparty/protobuf_2_6/bin/protoc --cpp_out=/home/lemaker/open-source/Chameleon/heldon-build/src/messages --proto_path=/home/lemaker/open-source/Chameleon/src/messages /home/lemaker/open-source/Chameleon/src/messages/scheduler.proto

src/messages/scheduler.pb.h: src/messages/scheduler.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate src/messages/scheduler.pb.h

src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o: src/messages/CMakeFiles/scheduler.dir/flags.make
src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o: src/messages/scheduler.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/scheduler.dir/scheduler.pb.cc.o -c /home/lemaker/open-source/Chameleon/heldon-build/src/messages/scheduler.pb.cc

src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/scheduler.dir/scheduler.pb.cc.i"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lemaker/open-source/Chameleon/heldon-build/src/messages/scheduler.pb.cc > CMakeFiles/scheduler.dir/scheduler.pb.cc.i

src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/scheduler.dir/scheduler.pb.cc.s"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lemaker/open-source/Chameleon/heldon-build/src/messages/scheduler.pb.cc -o CMakeFiles/scheduler.dir/scheduler.pb.cc.s

src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.requires:

.PHONY : src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.requires

src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.provides: src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.requires
	$(MAKE) -f src/messages/CMakeFiles/scheduler.dir/build.make src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.provides.build
.PHONY : src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.provides

src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.provides.build: src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o


# Object files for target scheduler
scheduler_OBJECTS = \
"CMakeFiles/scheduler.dir/scheduler.pb.cc.o"

# External object files for target scheduler
scheduler_EXTERNAL_OBJECTS =

src/messages/libscheduler.a: src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o
src/messages/libscheduler.a: src/messages/CMakeFiles/scheduler.dir/build.make
src/messages/libscheduler.a: src/messages/CMakeFiles/scheduler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libscheduler.a"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && $(CMAKE_COMMAND) -P CMakeFiles/scheduler.dir/cmake_clean_target.cmake
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/scheduler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/messages/CMakeFiles/scheduler.dir/build: src/messages/libscheduler.a

.PHONY : src/messages/CMakeFiles/scheduler.dir/build

src/messages/CMakeFiles/scheduler.dir/requires: src/messages/CMakeFiles/scheduler.dir/scheduler.pb.cc.o.requires

.PHONY : src/messages/CMakeFiles/scheduler.dir/requires

src/messages/CMakeFiles/scheduler.dir/clean:
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && $(CMAKE_COMMAND) -P CMakeFiles/scheduler.dir/cmake_clean.cmake
.PHONY : src/messages/CMakeFiles/scheduler.dir/clean

src/messages/CMakeFiles/scheduler.dir/depend: src/messages/scheduler.pb.cc
src/messages/CMakeFiles/scheduler.dir/depend: src/messages/scheduler.pb.h
	cd /home/lemaker/open-source/Chameleon/heldon-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lemaker/open-source/Chameleon /home/lemaker/open-source/Chameleon/src/messages /home/lemaker/open-source/Chameleon/heldon-build /home/lemaker/open-source/Chameleon/heldon-build/src/messages /home/lemaker/open-source/Chameleon/heldon-build/src/messages/CMakeFiles/scheduler.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/messages/CMakeFiles/scheduler.dir/depend

