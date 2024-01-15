cmake_minimum_required(VERSION 3.15)

project(csvDiff)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_executable(csvDiff csv_diff.cpp)