cmake_minimum_required(VERSION 3.21)

# Names
set(PROJECT_NAME "TUI Forum Client" CACHE STRING "Project Name")
set(EXECUTABLE_NAME client CACHE STRING "Executable Name")

# Dirs
set(DIR_SRC src CACHE PATH "Source Directory")
set(DIR_DIST dist CACHE PATH "Binary Directory")
set(DIR_INCLUDE include CACHE PATH "Include Directory")
set(DIR_SYSINC sysinc CACHE PATH "System Include Directory")
set(DIR_LIB lib CACHE PATH "Libraries Directory")

# Libraries
set (libraries
	# m (math)
	)

# Sources
set (sources
	main.cpp
	your_stdlib_is_garbage.cpp
	)

project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_POSITION_INDEPENDENT_CODE True)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

# Linker/Compiler Flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
	-fuse-ld=lld \
	")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-Wall -Wextra -Wno-unknown-pragmas -pedantic \
	-Wno-enum-compare -Wno-gnu-zero-variadic-macro-arguments \
	-Wno-c++11-extensions -Wno-c++14-extensions \
	-Wno-c++17-extensions -Wno-c++20-extensions \
	-fno-rtti -fexceptions \
	-Wno-unused-command-line-argument \
	")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fcolor-diagnostics")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
	-fno-omit-frame-pointer -fstack-protector-strong \
	-Og -ggdb3 -Werror -Wfatal-errors \
	")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
	-DNDEBUG \
	")

option(USE_ASAN "Use Address Sanitizer (Debug Only)" ON)
if(${USE_ASAN})
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
		-finstrument-functions \
		-fsanitize=address -fsanitize=undefined -fsanitize=leak \
		")
endif()

option(USE_GLIBCXX_DEBUG "STL Debug Mode (Debug Only)" ON)
if(${USE_GLIBCXX_DEBUG})
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
		-D_GLIBCXX_DEBUG \
		")
endif()

list(TRANSFORM sources PREPEND ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_SRC}/)

add_executable(${EXECUTABLE_NAME} ${sources})
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_INCLUDE})
target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE ${${PROJECT_NAME}_SOURCE_DIR}/${DIR_SYSINC})

set_property(TARGET ${EXECUTABLE_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_SOURCE_DIR}/${DIR_DIST}")

foreach(library ${libraries})
	target_link_libraries(${EXECUTABLE_NAME} ${library})
endforeach()

# Extra Stuff Starts Here
add_subdirectory(lib/TermOx)
target_link_libraries(${EXECUTABLE_NAME} TermOx)
