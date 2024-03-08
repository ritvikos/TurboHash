cmake_minimum_required(VERSION 3.10)
project(turbo-ht)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

include_directories(src/turbo src/util)

add_executable(turbo-ht src/turbo/main.cpp ${ARGV})

target_link_libraries(turbo-ht rt)
target_link_libraries(turbo-ht pthread)
target_link_libraries(turbo-ht tbb)
