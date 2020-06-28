cmake_minimum_required(VERSION 3.9)

project(ctrj)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

add_executable(test_reader test/test_reader.cpp ctrj/reader.hpp)
add_executable(test_bench test/test_bench.cpp)
add_executable(test_bench_rj test/test_bench_rj.cpp)
add_executable(test_citm_catalogs test/test_citm_catalogs.cpp)
