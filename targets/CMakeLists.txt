# Creates an executable named Main using the source file main.cpp. 
# This is the main program that will be compiled and can be run.
add_executable(Main main.cpp)

# Links the libsource library (created in the parent CMakeLists.txt) to the Main executable.
# PRIVATE keyword means the library is only for the executable's internal use.
target_link_libraries(Main PRIVATE libsource)