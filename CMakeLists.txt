cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

include(pico_sdk_import.cmake)

project(tic-tac-toe C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable
add_executable(tic-tac-toe main.cpp Game.cpp Move.cpp LCD_I2C.cpp TM1637.cpp Keypad.cpp Player.cpp Player.hpp BoardManager.cpp BoardManager.hpp Utility.hpp IPlayerStrategy.cpp IPlayerStrategy.hpp)
include_directories(${CMAKE_CURRENT_LIST_DIR})
pico_generate_pio_header(tic-tac-toe ${CMAKE_CURRENT_LIST_DIR}/TM1637.pio)

# Add program info
pico_set_program_name(tic-tac-toe "Tic-Tac-Toe LCD Game")
pico_set_program_version(tic-tac-toe "2.0")
pico_set_program_url(tic-tac-toe "https://github.com/cristiancristea00/tic-tac-toe")
pico_set_program_description(tic-tac-toe "Copyright (c) 2021 Cristian Cristea")

# Disable serial
pico_enable_stdio_uart(tic-tac-toe 0)
pico_enable_stdio_usb(tic-tac-toe 0)

# Add the libraries to the build
target_link_libraries(tic-tac-toe pico_stdlib pico_multicore hardware_i2c hardware_pio)

pico_add_extra_outputs(tic-tac-toe)

pico_set_float_implementation(tic-tac-toe pico)
pico_set_double_implementation(tic-tac-toe pico)