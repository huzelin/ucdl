# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_COMMAND = /usr/local/cmake/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/app/HPPS/hpps

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/app/HPPS/hpps/build

# Utility rule file for ompi.

# Include the progress variables for this target.
include thirdparty/CMakeFiles/ompi.dir/progress.make

ompi: thirdparty/CMakeFiles/ompi.dir/build.make
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Build MPI"
	cd /home/app/HPPS/hpps/build/thirdparty && echo OMPI\ Is\ Builded
.PHONY : ompi

# Rule to build all files generated by this target.
thirdparty/CMakeFiles/ompi.dir/build: ompi

.PHONY : thirdparty/CMakeFiles/ompi.dir/build

thirdparty/CMakeFiles/ompi.dir/clean:
	cd /home/app/HPPS/hpps/build/thirdparty && $(CMAKE_COMMAND) -P CMakeFiles/ompi.dir/cmake_clean.cmake
.PHONY : thirdparty/CMakeFiles/ompi.dir/clean

thirdparty/CMakeFiles/ompi.dir/depend:
	cd /home/app/HPPS/hpps/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/app/HPPS/hpps /home/app/HPPS/hpps/thirdparty /home/app/HPPS/hpps/build /home/app/HPPS/hpps/build/thirdparty /home/app/HPPS/hpps/build/thirdparty/CMakeFiles/ompi.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : thirdparty/CMakeFiles/ompi.dir/depend
