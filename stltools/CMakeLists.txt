project(stltools)
cmake_minimum_required(VERSION 2.8)

include_directories(../vcglib)

add_executable(stlconvert stlconvert.cpp)
add_executable(manifoldtest manifoldtest.cpp)
