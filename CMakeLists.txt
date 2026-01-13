cmake_minimum_required(VERSION 3.10)
project(dretsim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2 -g -fno-omit-frame-pointer -march=native")

add_executable(dretsim src/main.cpp)
