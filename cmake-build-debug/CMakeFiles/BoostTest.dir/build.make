# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/BoostTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/BoostTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/BoostTest.dir/flags.make

CMakeFiles/BoostTest.dir/main.cpp.o: CMakeFiles/BoostTest.dir/flags.make
CMakeFiles/BoostTest.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/BoostTest.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/BoostTest.dir/main.cpp.o -c /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/main.cpp

CMakeFiles/BoostTest.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BoostTest.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/main.cpp > CMakeFiles/BoostTest.dir/main.cpp.i

CMakeFiles/BoostTest.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BoostTest.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/main.cpp -o CMakeFiles/BoostTest.dir/main.cpp.s

# Object files for target BoostTest
BoostTest_OBJECTS = \
"CMakeFiles/BoostTest.dir/main.cpp.o"

# External object files for target BoostTest
BoostTest_EXTERNAL_OBJECTS =

BoostTest: CMakeFiles/BoostTest.dir/main.cpp.o
BoostTest: CMakeFiles/BoostTest.dir/build.make
BoostTest: /usr/local/lib/libboost_system.dylib
BoostTest: /usr/local/lib/libboost_filesystem.dylib
BoostTest: CMakeFiles/BoostTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable BoostTest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BoostTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/BoostTest.dir/build: BoostTest

.PHONY : CMakeFiles/BoostTest.dir/build

CMakeFiles/BoostTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/BoostTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/BoostTest.dir/clean

CMakeFiles/BoostTest.dir/depend:
	cd /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/cmake-build-debug /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/cmake-build-debug /Users/jonathanhirsch/Dropbox/UBCUAS/skypasta/cmake-build-debug/CMakeFiles/BoostTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/BoostTest.dir/depend

