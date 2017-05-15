cmake_minimum_required(VERSION 3.7)
project(move_generator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp move_generator.cpp)
add_executable(move_generator ${SOURCE_FILES})