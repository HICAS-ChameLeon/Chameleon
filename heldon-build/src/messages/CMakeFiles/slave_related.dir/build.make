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
include src/messages/CMakeFiles/slave_related.dir/depend.make

# Include the progress variables for this target.
include src/messages/CMakeFiles/slave_related.dir/progress.make

# Include the compile flags for this target's objects.
include src/messages/CMakeFiles/slave_related.dir/flags.make

src/messages/slave_related.pb.cc:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating slave_related.pb.cc, slave_related.pb.h"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && ../../3rdparty/protobuf_2_6/bin/protoc --cpp_out=/home/lemaker/open-source/Chameleon/heldon-build/src/messages --proto_path=/home/lemaker/open-source/Chameleon/src/messages /home/lemaker/open-source/Chameleon/src/messages/slave_related.proto

src/messages/slave_related.pb.h: src/messages/slave_related.pb.cc
	@$(CMAKE_COMMAND) -E touch_nocreate src/messages/slave_related.pb.h

src/messages/CMakeFiles/slave_related.dir/slave_related.pb.cc.o: src/messages/CMakeFiles/slave_related.dir/flags.make
src/messages/CMakeFiles/slave_related.dir/slave_related.pb.cc.o: src/messages/slave_related.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/messages/CMakeFiles/slave_related.dir/slave_related.pb.cc.o"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/slave_related.dir/slave_related.pb.cc.o -c /home/lemaker/open-source/Chameleon/heldon-build/src/messages/slave_related.pb.cc

src/messages/CMakeFiles/slave_related.dir/slave_related.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/slave_related.dir/slave_related.pb.cc.i"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lemaker/open-source/Chameleon/heldon-build/src/messages/slave_related.pb.cc > CMakeFiles/slave_related.dir/slave_related.pb.cc.i

src/messages/CMakeFiles/slave_related.dir/slave_related.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/slave_related.dir/slave_related.pb.cc.s"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lemaker/open-source/Chameleon/heldon-build/src/messages/slave_related.pb.cc -o CMakeFiles/slave_related.dir/slave_related.pb.cc.s

# Object files for target slave_related
slave_related_OBJECTS = \
"CMakeFiles/slave_related.dir/slave_related.pb.cc.o"

# External object files for target slave_related
slave_related_EXTERNAL_OBJECTS =

src/messages/libslave_related.a: src/messages/CMakeFiles/slave_related.dir/slave_related.pb.cc.o
src/messages/libslave_related.a: src/messages/CMakeFiles/slave_related.dir/build.make
src/messages/libslave_related.a: src/messages/CMakeFiles/slave_related.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lemaker/open-source/Chameleon/heldon-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libslave_related.a"
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && $(CMAKE_COMMAND) -P CMakeFiles/slave_related.dir/cmake_clean_target.cmake
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/slave_related.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/messages/CMakeFiles/slave_related.dir/build: src/messages/libslave_related.a

.PHONY : src/messages/CMakeFiles/slave_related.dir/build

src/messages/CMakeFiles/slave_related.dir/clean:
	cd /home/lemaker/open-source/Chameleon/heldon-build/src/messages && $(CMAKE_COMMAND) -P CMakeFiles/slave_related.dir/cmake_clean.cmake
.PHONY : src/messages/CMakeFiles/slave_related.dir/clean

src/messages/CMakeFiles/slave_related.dir/depend: src/messages/slave_related.pb.cc
src/messages/CMakeFiles/slave_related.dir/depend: src/messages/slave_related.pb.h
	cd /home/lemaker/open-source/Chameleon/heldon-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lemaker/open-source/Chameleon /home/lemaker/open-source/Chameleon/src/messages /home/lemaker/open-source/Chameleon/heldon-build /home/lemaker/open-source/Chameleon/heldon-build/src/messages /home/lemaker/open-source/Chameleon/heldon-build/src/messages/CMakeFiles/slave_related.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/messages/CMakeFiles/slave_related.dir/depend

