# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/home/cristian/Documents/Pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Tic_Tac_Toe C CXX ASM)

set(PICO_DEOPTIMIZED_DEBUG 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Tic_Tac_Toe main.cpp Game.cpp Game.hpp Action.cpp Action.hpp)

pico_set_program_name(Tic_Tac_Toe "LCD")
pico_set_program_version(Tic_Tac_Toe "0.1")

pico_enable_stdio_uart(Tic_Tac_Toe 1)
pico_enable_stdio_usb(Tic_Tac_Toe 0)

# Add the standard library to the build
target_link_libraries(Tic_Tac_Toe pico_stdlib)

# Add any user requested libraries
target_link_libraries(Tic_Tac_Toe hardware_i2c)

pico_add_extra_outputs(Tic_Tac_Toe)
