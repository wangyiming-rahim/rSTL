cmake_minimum_required(VERSION 3.21)
project(rSTL)

set(CMAKE_CXX_STANDARD 20)

# add_executable(rSTL main.cpp internal/config.h include/utility.h internal/tuple_fwd_decls.h include/tuple.h internal/integer_sequence.h internal/piecewise_construct_t.h include/shared_ptr.h internal/thread_support.h include/allocator.h internal/smart_ptr.h internal/enable_shared.h)
add_executable(rSTL main.cpp)

include_directories(include)

target_include_directories(rSTL PUBLIC include)