# This CMakeLists.txt file is only configured for debugging
cmake_minimum_required(VERSION 2.6)
project(NOT_ENGLISH)

add_definitions(-std=c++11 -g -Wall)

# For boost:
find_package(Boost 1.54 REQUIRED)
# link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

set(target_file ./bin/NotEnglish)
file(GLOB sources *.cpp)
add_executable(${target_file} ${sources})
