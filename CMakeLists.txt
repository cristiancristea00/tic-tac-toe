# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

project(tic-tac-toe C CXX ASM)

set(PICO_DEOPTIMIZED_DEBUG 1)

# Add executable. Default name is the project name, version 0.1

add_executable(tic-tac-toe main.cpp Game.cpp Game.hpp Action.cpp Action.hpp)

pico_set_program_name(tic-tac-toe "Tic Tac Toe")
pico_set_program_version(tic-tac-toe "0.1")

pico_enable_stdio_uart(tic-tac-toe 1)
pico_enable_stdio_usb(tic-tac-toe 0)

# Add the standard library to the build
target_link_libraries(tic-tac-toe pico_stdlib)

# Add any user requested libraries
target_link_libraries(tic-tac-toe hardware_i2c)

pico_add_extra_outputs(tic-tac-toe)
