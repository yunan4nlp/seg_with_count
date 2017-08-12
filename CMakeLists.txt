project(seg_with_count)
cmake_minimum_required(VERSION 2.7 FATAL_ERROR)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    SET( CMAKE_CXX_FLAGS  "-w -msse3 -funroll-loops -std=c++11 -O0" )
else()
    SET( CMAKE_CXX_FLAGS  "-w -msse3 -funroll-loops -std=c++11 -O3" )
endif()

add_subdirectory(src)
