cmake_minimum_required(VERSION 3.15)

project(csvDiff)

set(CMAKE_CXX_STANDARD 14)

set(PRE_CONFIGURE_FILE "${CMAKE_SOURCE_DIR}/cmake/git.hpp.in")
set(POST_CONFIGURE_FILE "${CMAKE_SOURCE_DIR}/git.hpp")
include(${CMAKE_SOURCE_DIR}/cmake/git_watcher.cmake)

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_executable(csvDiff ${CMAKE_SOURCE_DIR}/csv_diff.cpp)
add_dependencies(csvDiff check_git)