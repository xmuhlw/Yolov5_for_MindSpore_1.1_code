# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/HwHiAiUser/samples/notebooks/RefineDet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/HwHiAiUser/samples/notebooks/RefineDet/build

# Include any dependencies generated for this target.
include CMakeFiles/refinedet.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/refinedet.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/refinedet.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/refinedet.dir/flags.make

CMakeFiles/refinedet.dir/main.cpp.o: CMakeFiles/refinedet.dir/flags.make
CMakeFiles/refinedet.dir/main.cpp.o: ../main.cpp
CMakeFiles/refinedet.dir/main.cpp.o: CMakeFiles/refinedet.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/HwHiAiUser/samples/notebooks/RefineDet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/refinedet.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/refinedet.dir/main.cpp.o -MF CMakeFiles/refinedet.dir/main.cpp.o.d -o CMakeFiles/refinedet.dir/main.cpp.o -c /home/HwHiAiUser/samples/notebooks/RefineDet/main.cpp

CMakeFiles/refinedet.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/refinedet.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/HwHiAiUser/samples/notebooks/RefineDet/main.cpp > CMakeFiles/refinedet.dir/main.cpp.i

CMakeFiles/refinedet.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/refinedet.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/HwHiAiUser/samples/notebooks/RefineDet/main.cpp -o CMakeFiles/refinedet.dir/main.cpp.s

CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o: CMakeFiles/refinedet.dir/flags.make
CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o: ../RefineDetDetection/RefineDetDetection.cpp
CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o: CMakeFiles/refinedet.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/HwHiAiUser/samples/notebooks/RefineDet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o -MF CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o.d -o CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o -c /home/HwHiAiUser/samples/notebooks/RefineDet/RefineDetDetection/RefineDetDetection.cpp

CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/HwHiAiUser/samples/notebooks/RefineDet/RefineDetDetection/RefineDetDetection.cpp > CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.i

CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/HwHiAiUser/samples/notebooks/RefineDet/RefineDetDetection/RefineDetDetection.cpp -o CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.s

CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o: CMakeFiles/refinedet.dir/flags.make
CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o: ../RefineDetPostProcess/RefineDetPostProcess.cpp
CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o: CMakeFiles/refinedet.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/HwHiAiUser/samples/notebooks/RefineDet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o -MF CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o.d -o CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o -c /home/HwHiAiUser/samples/notebooks/RefineDet/RefineDetPostProcess/RefineDetPostProcess.cpp

CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/HwHiAiUser/samples/notebooks/RefineDet/RefineDetPostProcess/RefineDetPostProcess.cpp > CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.i

CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/HwHiAiUser/samples/notebooks/RefineDet/RefineDetPostProcess/RefineDetPostProcess.cpp -o CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.s

# Object files for target refinedet
refinedet_OBJECTS = \
"CMakeFiles/refinedet.dir/main.cpp.o" \
"CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o" \
"CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o"

# External object files for target refinedet
refinedet_EXTERNAL_OBJECTS =

../refinedet: CMakeFiles/refinedet.dir/main.cpp.o
../refinedet: CMakeFiles/refinedet.dir/RefineDetDetection/RefineDetDetection.cpp.o
../refinedet: CMakeFiles/refinedet.dir/RefineDetPostProcess/RefineDetPostProcess.cpp.o
../refinedet: CMakeFiles/refinedet.dir/build.make
../refinedet: CMakeFiles/refinedet.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/HwHiAiUser/samples/notebooks/RefineDet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable ../refinedet"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/refinedet.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/refinedet.dir/build: ../refinedet
.PHONY : CMakeFiles/refinedet.dir/build

CMakeFiles/refinedet.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/refinedet.dir/cmake_clean.cmake
.PHONY : CMakeFiles/refinedet.dir/clean

CMakeFiles/refinedet.dir/depend:
	cd /home/HwHiAiUser/samples/notebooks/RefineDet/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/HwHiAiUser/samples/notebooks/RefineDet /home/HwHiAiUser/samples/notebooks/RefineDet /home/HwHiAiUser/samples/notebooks/RefineDet/build /home/HwHiAiUser/samples/notebooks/RefineDet/build /home/HwHiAiUser/samples/notebooks/RefineDet/build/CMakeFiles/refinedet.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/refinedet.dir/depend
