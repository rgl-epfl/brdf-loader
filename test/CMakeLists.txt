cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

include_directories(..)

# ------------------------------------------------------------------------------
project (acq)

# ------------------------------------------------------------------------------
add_executable(hello hello.cpp)
add_executable(hello_rgb hello_rgb.cpp)
