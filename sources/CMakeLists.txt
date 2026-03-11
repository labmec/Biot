# Adds the current source directory to the include path for libsource.
target_include_directories(libsource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SRC CONFIGURE_DEPENDS "*.cpp" "*.c" "*.hpp")

file(GLOB HEADERS CONFIGURE_DEPENDS "*.h")

target_sources(libsource PRIVATE ${SRC} ${HEADERS})