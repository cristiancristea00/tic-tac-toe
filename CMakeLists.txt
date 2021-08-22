# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Pull in Raspberry Pi Pico SDK (must be before project)
if (NOT DEFINED ENV{PICO_SDK_PATH})
    SET(PICO_SDK_FETCH_FROM_GIT 1)
endif ()

include(pico_sdk_import.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

project(tic-tac-toe C CXX ASM)

# Add executable
add_executable(tic-tac-toe main.cpp Game.cpp Game.hpp Action.cpp Action.hpp LCD_I2C.cpp LCD_I2C.hpp
        TM1637.cpp TM1637.hpp Keypad.cpp Keypad.hpp)
pico_generate_pio_header(tic-tac-toe ${CMAKE_CURRENT_LIST_DIR}/TM1637.pio)

pico_set_program_name(tic-tac-toe "Tic Tac Toe LCD Game")
pico_set_program_version(tic-tac-toe "0.3")
pico_set_program_url(tic-tac-toe "https://github.com/cristiancristea00/tic-tac-toe")
pico_set_program_description(tic-tac-toe "Copyright (c) 2021 Cristian Cristea")


pico_enable_stdio_uart(tic-tac-toe 1)
pico_enable_stdio_usb(tic-tac-toe 0)

# Add the libraries to the build
target_link_libraries(tic-tac-toe pico_stdlib hardware_i2c hardware_pio)

pico_add_extra_outputs(tic-tac-toe)

pico_set_float_implementation(tic-tac-toe pico)
pico_set_double_implementation(tic-tac-toe pico)