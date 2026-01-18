cmake_minimum_required(VERSION 3.10)

if(POLICY CMP0072)
	cmake_policy(SET CMP0072 NEW)
endif()

project(dretsim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -g -fno-omit-frame-pointer -march=native")

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Create GLAD library
add_library(glad external/src/glad.c)
target_include_directories(glad PUBLIC external/glad)

# Create executable
add_executable(dretsim src/main.cpp)

# Link libraries
target_link_libraries(dretsim OpenGL::GL glfw glad)
