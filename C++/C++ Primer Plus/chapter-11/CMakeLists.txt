cmake_minimum_required(VERSION 3.0)

project(TEST)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -o2 -Wall")
# add_compile_options(-Wall -std=c++11 -O2)

set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR}/include)# 顶层目录：./include

add_executable(my_cmake_exe 10-7-2.cpp 10-7-1.cpp)