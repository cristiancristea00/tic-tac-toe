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
    using data = uint32_t;

 private:

    static constexpr byte BYTE_SIZE = 8;
    static constexpr byte MAX_DIGITS = 4;
    static constexpr byte MAX_BRIGHTNESS = 7;
    static constexpr byte BRIGHTNESS_BASE = 0x88;
    static constexpr byte WRITE_MODE = 0x40;
    static constexpr byte WRITE_ADDRESS = 0xC0;
    static constexpr byte BASE_TEN = 10;

    static constexpr std::array DIGIT_TO_SEGMENTS {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

    bool is_colon {};
    byte brightness {};
    data current_segments {};

    PIO pio {};
    byte state_machine {};
    pio_sm_config state_machine_config {};

    /**
     * Sets up and configures the state machine.
     *
     * @param DIO The data input/output pin
     * @param CLK The clock pin
     */
    inline void Init(byte DIO, byte CLK) noexcept;

    /**
     * Sets up the state machine's clock divider.
     */
    inline void Set_Clock_Divider() noexcept;

    /**
     * Send four bytes of value to the state machine.
     *
     * @param value The 4 bytes data to be sent
     */
    void Send_4_Bytes(data value) const noexcept;

    /**
     * Converts a number to the bytes corresponding to the segments on the
     * display that need to light up. If the input is more than four digits,
     * then the least significant digits will be cut off. You can also cut off
     * parts with a bitmask.
     *
     * @param number The number to be converted
     * @param bitmask The optional bitmask
     * @return The data that correspond to the lit up segments
     */
    static auto Number_To_Segments(data number, data bitmask = 0) noexcept -> data;

    /**
     * Helper for getting the segments representation for a two digit number.
     *
     * @param number The number to be converted
     * @param leading_zeros Optional leading zeros
     * @return The data that correspond to the lit up segments
     */
    static auto Two_Digits_To_Segment(data number, bool leading_zeros = false) noexcept -> data;

 public:

    /**
     * [Constructor] Initialises the two pins using the PIO instance.
     *
     * @param DIO The data input/output pin
     * @param CLK The clock pin
     * @param pio The PIO instance
     */
    TM1637(byte DIO, byte CLK, PIO pio) noexcept;

    /**
     * Sets the brightness brightness_level. The minimum brightness_level is 0
     * and the maximum is 7.
     *
     * @param brightness_level The desired brightness brightness_level
     */
    void SetBrightness(byte brightness_level) noexcept;

    /**
     * Display a number on all four digits.
     *
     * @param number The number to be displayed
     * @param leading_zeros Optional leading zeros
     */
    void Display(int16_t number, bool leading_zeros = false) noexcept;

    /**
     * Display a number on the most significant two digits.
     *
     * @param number The number to be displayed
     * @param leading_zeros Optional leading zeros
     */
    void DisplayLeft(data number, bool leading_zeros = false) noexcept;

    /**
     * Display a number on the least significant two digits.
     *
     * @param number The number to be displayed
     * @param leading_zeros Optional leading zeros
     */
    void DisplayRight(data number, bool leading_zeros = false) noexcept;

    /**
     * Turns on the middle colon.
     */
    void ColonOn() noexcept;

    /**
     * Turns off the middle colon.
     */
    void ColonOff() noexcept;

    /**
     * Clears the display.
     */
    void Clear() const noexcept;
};

