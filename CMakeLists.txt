cmake_minimum_required(VERSION 3.19)
project(Tic_Tac_Toe)

set(CMAKE_CXX_STANDARD 17)

add_executable(Tic_Tac_Toe main.cpp Game.cpp Game.hpp Action.cpp Action.hpp)

find_package(fmt)
target_link_libraries(Tic_Tac_Toe fmt::fmt)