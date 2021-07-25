/*******************************************************************************
 * @file TM1637.hpp
 * @author Cristian Cristea
 * @date July 21, 2021
 * @brief Header file for the TM1637 class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <hardware/clocks.h>

#include <TM1637.pio.h>

#include <algorithm>
#include <cstdint>

class TM1637
{
 public:
    using byte = uint8_t;
    using value = uint32_t;

 private:

    static constexpr byte BYTE_SIZE = 8;
    static constexpr byte MAX_DIGITS = 4;
    static constexpr byte MAX_BRIGHTNESS = 7;
    static constexpr byte BRIGHTNESS_BASE = 0x88;
    static constexpr byte WRITE_MODE = 0x40;
    static constexpr byte WRITE_ADDRESS = 0xC0;

    static constexpr byte DIGIT_TO_SEGMENTS[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

    byte brightness {};
    PIO pio {};
    byte state_machine {};
    pio_sm_config state_machine_config {};
    value current_segments {};

    inline void Init(byte DIO, byte CLK) noexcept;
    inline void Set_Clock_Divider() const noexcept;
    void Send_4_Bytes(value data) const noexcept;
    static value Number_To_Segments(value number, value bit_mask) noexcept;
    static unsigned long Two_Digits_To_Segment(value number, bool leading_zeros) noexcept;

 public:
    TM1637(byte DIO, byte CLK, PIO pio) noexcept;
    void SetBrightness(byte value) noexcept;
    void Display(int16_t number, bool leading_zeros) const noexcept;
    void DisplayLeft(value number, bool leading_zeros) noexcept;
    void DisplayRight(value number, bool leading_zeros) noexcept;
    void ColonOn() noexcept;
    void ColonOff() noexcept;
    void Clear() const noexcept;
};

