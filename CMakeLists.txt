cmake_minimum_required(VERSION 3.13)

# Set C and C++ and versions
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Set default build type to Release
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    message("Using by default the Release build")
endif ()

# Include the Raspberry Pi Pico SDK import script
include(pico_sdk_import.cmake)

# Project
project(tic-tac-toe C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable
file(GLOB TIC_TAC_TOE_SOURCES "src/*.cpp")
add_executable(tic-tac-toe ${TIC_TAC_TOE_SOURCES})
target_include_directories(tic-tac-toe PRIVATE include)
pico_generate_pio_header(tic-tac-toe ${CMAKE_CURRENT_LIST_DIR}/pio/TM1637.pio)

# Add program info
pico_set_program_name(tic-tac-toe "Tic-Tac-Toe LCD Game")
pico_set_program_version(tic-tac-toe "1.2.0")
pico_set_program_url(tic-tac-toe "https://github.com/cristiancristea00/tic-tac-toe")
pico_set_program_description(tic-tac-toe "Copyright (c) 2021 Cristian Cristea")

# Disable serial
pico_enable_stdio_uart(tic-tac-toe 0)
pico_enable_stdio_usb(tic-tac-toe 0)

# Add the libraries to the build
target_link_libraries(tic-tac-toe pico_stdlib pico_multicore hardware_i2c hardware_pio)

# Add pico extras
pico_add_extra_outputs(tic-tac-toe)

# Set float and double implementation
pico_set_float_implementation(tic-tac-toe pico)
pico_set_double_implementation(tic-tac-toe pico)

# Set Debug build compiler arguments
set(CMAKE_CXX_FLAGS_DEBUG "-pipe -g -O0 -Wfatal-errors -Wpedantic -Wall -Wextra -Wconversion -Wshadow=local -Wdouble-promotion -Wformat=2 -Wformat-overflow=2 -Wformat-nonliteral -Wformat-security -Wformat-truncation=2 -Wnull-dereference -Wimplicit-fallthrough=3 -Wshift-overflow=2 -Wswitch-default -Wunused-parameter -Wunused-const-variable=2 -Wstrict-overflow=4 -Wstringop-overflow=3 -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wmissing-noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wmissing-format-attribute -Wsuggest-attribute=cold -Walloc-zero -Walloca -Wattribute-alias=2 -Wduplicated-branches -Wcast-qual")
set(CMAKE_C_FLAGS_DEBUG "-pipe -g -O0 -Wfatal-errors -Wpedantic -Wall -Wextra -Wconversion -Wshadow=local -Wdouble-promotion -Wformat=2 -Wformat-overflow=2 -Wformat-nonliteral -Wformat-security -Wformat-truncation=2 -Wnull-dereference -Wimplicit-fallthrough=3 -Wshift-overflow=2 -Wswitch-default -Wunused-parameter -Wunused-const-variable=2 -Wstrict-overflow=4 -Wstringop-overflow=3 -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wmissing-noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wmissing-format-attribute -Wsuggest-attribute=cold -Walloc-zero -Walloca -Wattribute-alias=2 -Wduplicated-branches -Wcast-qual")

# Set Release build compiler arguments
set(CMAKE_CXX_FLAGS_RELEASE "-pipe -Os -fno-builtin")
set(CMAKE_C_FLAGS_RELEASE "-pipe -Os -fno-builtin")
