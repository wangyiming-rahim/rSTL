cmake_minimum_required(VERSION 3.21)
project(rSTL)

set(CMAKE_CXX_STANDARD 20)

add_executable(rSTL main.cpp internal/config.h include/utility.h internal/tuple_fwd_decls.h include/tuple.h internal/integer_sequence.h internal/piecewise_construct_t.h)
