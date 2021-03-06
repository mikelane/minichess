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

set(SOURCE_FILES main.cpp bitboard_tables.h  Player.cpp Player.h Random_Player.cpp Random_Player.h Testing_Player.cpp Testing_Player.h Negamax_Player.cpp Negamax_Player.h AB_Player.cpp AB_Player.h Negamax_Result.cpp Negamax_Result.h Move.cpp Move.h AB_ID_Player.cpp AB_ID_Player.h AB_ID_TT_Player.cpp AB_ID_TT_Player.h TTable_Entry.cpp TTable_Entry.h TTable.cpp TTable.h Zobrist_Table.cpp Zobrist_Table.h)
add_executable(move_generator ${SOURCE_FILES})

target_include_directories(move_generator PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(move_generator PUBLIC ${ZeroMQ_LIBRARY})