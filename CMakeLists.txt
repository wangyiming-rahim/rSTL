cmake_minimum_required(VERSION 3.21)
project(rSTL)

set(CMAKE_CXX_STANDARD 20)

add_executable(rSTL main.cpp include/config.h include/utility.h include/type_traits.h)
