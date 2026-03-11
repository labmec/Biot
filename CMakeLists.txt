cmake_minimum_required(VERSION 3.14)

project (Biot LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

# Searches for the NeoPZ external library, which is required
find_package(NeoPZ REQUIRED HINTS ../NeoPZ_install ../neopz_install)
# find_package(NeoPZ REQUIRED HINTS ${CMAKE_SOURCE_DIR}/../neopz_install_debug/ ${CMAKE_SOURCE_DIR}/neopz_install/)


option(USING_LAPACK OFF)
if(USING_LAPACK)
    add_compile_definitions(USING_LAPACK)
endif()


file(GLOB_RECURSE PZ_SRC
     "../neopz/*.h"
     "../neopz/*.cpp"
     "../NeoPzDevelop/*.h"
     "../NeoPzDevelop/*.cpp"
)
add_custom_target(GlobalFiles SOURCES ${PZ_SRC})

add_library(libsource SHARED "")

# Links the NeoPZ library to libsource publicly
target_link_libraries(libsource PUBLIC NeoPZ::pz)

target_include_directories(libsource PUBLIC ${PZ_INCLUDE_DIRS})

set_target_properties(libsource PROPERTIES LINKER_LANGUAGE CXX)

# Includes the sources and targets subdirectories
add_subdirectory(sources)
add_subdirectory(targets)