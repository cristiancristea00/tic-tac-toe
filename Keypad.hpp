/*******************************************************************************
 * @file Keypad.hpp
 * @author Cristian Cristea
 * @date August 20, 2021
 * @brief Header file for the Keypad class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <hardware/gpio.h>
#include <pico/time.h>

#include <cstdint>
#include <array>

enum class Key
{
    KEY1, KEY2, KEY3, KEY4,
    KEY5, KEY6, KEY7, KEY8,
    KEY9, KEY10, KEY11, KEY12,
    KEY13, KEY14, KEY15, KEY16,
    UNKOWN
};

class Keypad
{
 private:
    using byte = uint8_t;

    static constexpr byte MAX_SIZE = 4;

 public:
    using array = std::array<byte, MAX_SIZE>;

 private:
    static constexpr bool LOW = false;
    static constexpr bool HIGH = true;

    array rows;
    array columns;

    static constexpr Key KEYS[MAX_SIZE][MAX_SIZE] = {{Key::KEY1, Key::KEY2, Key::KEY3, Key::KEY4},
                                                     {Key::KEY5, Key::KEY6, Key::KEY7, Key::KEY8},
                                                     {Key::KEY9, Key::KEY10, Key::KEY11, Key::KEY12},
                                                     {Key::KEY13, Key::KEY14, Key::KEY15, Key::KEY16}};

    inline void Init() noexcept;

 public:
    Keypad(array const & rows, array const & columns) noexcept;
    [[nodiscard]] Key Poll() const noexcept;
};

