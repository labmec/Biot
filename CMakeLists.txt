cmake_minimum_required(VERSION 3.14)

# Declares the project name as "ProjectName" and specifies that it will use C and C++
project (ProjectName LANGUAGES C CXX)

# Sets the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Searches for the NeoPZ external library, which is required
find_package(NeoPZ REQUIRED HINTS ../NeoPZ_install ../neopz_install)
# find_package(NeoPZ REQUIRED HINTS ${CMAKE_SOURCE_DIR}/../neopz_install_debug/ ${CMAKE_SOURCE_DIR}/neopz_install/)

# If using Lapack
option(USING_LAPACK OFF)
if(USING_LAPACK)
    add_compile_definitions(USING_LAPACK)
endif()

# Recursively finds all header (.h) and source (.cpp) files in the neopz directories. Creates a custom target named GlobalFiles
file(GLOB_RECURSE PZ_SRC
     "../neopz/*.h"
     "../neopz/*.cpp"
     "../NeoPzDevelop/*.h"
     "../NeoPzDevelop/*.cpp"
)
add_custom_target(GlobalFiles SOURCES ${PZ_SRC})

# Creates a shared library (dynamic library)
add_library(libsource SHARED "")

# Links the NeoPZ library to libsource publicly, meaning any project that links 
# against libsource will also link against NeoPZ.
target_link_libraries(libsource PUBLIC NeoPZ::pz)

# Adds the NeoPZ include directories to libsource so it can find the headers
target_include_directories(libsource PUBLIC ${PZ_INCLUDE_DIRS})

# Explicitly tells CMake to use the C++ linker for libsource
set_target_properties(libsource PROPERTIES LINKER_LANGUAGE CXX)

# Includes the sources and targets subdirectories
add_subdirectory(sources)
add_subdirectory(targets)