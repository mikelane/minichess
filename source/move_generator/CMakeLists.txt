cmake_minimum_required(VERSION 3.7)
project(move_generator)

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

set(SOURCE_FILES main.cpp move_generator.cpp)
add_executable(move_generator ${SOURCE_FILES})

target_include_directories(move_generator PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(move_generator PUBLIC ${ZeroMQ_LIBRARY})