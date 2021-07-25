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
#include <hardware/pio.h>

#include <TM1637.pio.h>

#include <cstdint>

class TM1637
{
 public:
    using byte = uint8_t;
    using value = uint32_t;

 private:

    static constexpr byte WRITE_MODE = 0x40;
    static constexpr byte WRITE_ADDRESS = 0xC0;
    static constexpr byte BRIGHTNESS_BASE = 0x88;
    static constexpr byte MAX_DIGITS = 4;
    static constexpr byte COLON = 0x80;

    static constexpr byte DIGIT_TO_SEGMENTS[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

    byte DIO {};
    byte CLK {};
    byte brightness {};

    PIO pio {};
    byte state_machine {};
    pio_sm_config state_machine_config {};

    inline void Refresh_Frequency() const noexcept;
    inline void Refresh_Display() const noexcept;
    void Send_2_Bytes(value start_position, value data) const noexcept;
    void Send_4_Bytes(value start_position, value data) const noexcept;
    static value Number_To_Segments(value number, value bit_mask) noexcept;
    static unsigned long Two_Digits_To_Segment(value number, bool leading_zeros) noexcept;

 public:
    TM1637(byte DIO, byte CLK, PIO pio) noexcept;
    void DisplayLeft(value number, bool leading_zeros) const noexcept;
    void DisplayRight(value number, bool leading_zeros) const noexcept;
    void DisplayColon() const noexcept;
};

