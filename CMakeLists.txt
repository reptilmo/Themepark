cmake_minimum_required(VERSION 3.23)

project("Project")

if (NOT CMAKE_BUILD_TYPE)
    message("Build type not set, defaulting to debug!")
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_definitions(LINUX_BUILD DEBUG_BUILD TRACK_HEAP)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/glad)
add_subdirectory(source)
