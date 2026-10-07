cmake_minimum_required(VERSION 3.20)
project(sail LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(sail src/main.cpp)
target_compile_options(sail PRIVATE -Wall -Wextra -Wpedantic)
