/*******************************************************************************
 * @file Lcd.hpp
 * @author Cristian Cristea
 * @date July 08, 2021
 * @brief Header file for the LCD_I2C class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <string_view>
#include <cstdint>

#include <hardware/gpio.h>
#include <hardware/i2c.h>

class LCD_I2C
{
 public:
    using byte = uint8_t;

 private:
    // Commands
    static constexpr byte CLEAR_DISPLAY = 0x01;
    static constexpr byte RETURN_HOME = 0x02;
    static constexpr byte ENTRY_MODE_SET = 0x04;
    static constexpr byte DISPLAY_CONTROL = 0x08;
    static constexpr byte CURSOR_SHIFT = 0x10;
    static constexpr byte FUNCTION_SET = 0x20;
    static constexpr byte SET_CGRAM_ADDR = 0x40;
    static constexpr byte SET_DDRAM_ADDR = 0x80;

    // Flags for display entry mode set
    static constexpr byte ENTRY_RIGHT = 0x00;
    static constexpr byte ENTRY_LEFT = 0x02;
    static constexpr byte ENTRY_SHIFT_INCREMENT = 0x01;
    static constexpr byte ENTRY_SHIFT_DECREMENT = 0x00;

    // Flags for display on/off control
    static constexpr byte DISPLAY_ON = 0x04;
    static constexpr byte DISPLAY_OFF = 0x00;
    static constexpr byte CURSOR_ON = 0x02;
    static constexpr byte CURSOR_OFF = 0x00;
    static constexpr byte BLINK_ON = 0x01;
    static constexpr byte BLINK_OFF = 0x00;

    // Flags for cursor or display shift
    static constexpr byte DISPLAY_MOVE = 0x08;
    static constexpr byte CURSOR_MOVE = 0x00;
    static constexpr byte MOVE_RIGHT = 0x04;
    static constexpr byte MOVE_LEFT = 0x00;

    // Flags for function set
    static constexpr byte MODE_8_BIT = 0x10;
    static constexpr byte MODE_4_BIT = 0x00;
    static constexpr byte LINE_2 = 0x08;
    static constexpr byte LINE_1 = 0x00;
    static constexpr byte DOTS_5x10 = 0x04;
    static constexpr byte DOTS_5x8 = 0x00;

    // Flags for backlight control
    static constexpr byte BACKLIGHT = 0x08;
    static constexpr byte NO_BACKLIGHT = 0x00;

    // Special flags
    static constexpr byte ENABLE = 0x04;
    static constexpr byte READ_WRITE = 0x02;
    static constexpr byte REGISTER_SELECT = 0x01;

    // Command flags
    static constexpr byte COMMAND = 0x00;
    static constexpr byte CHAR = 0x01;

    byte address {};
    byte columns {};
    byte rows {};
    byte backlight {};
    byte display_function {};
    byte display_control {};
    byte display_mode {};

    i2c_inst * I2C_instance = nullptr;

    inline void I2C_Write_Byte(byte val) const noexcept;
    void Pulse_Enable(byte val) const noexcept;
    inline void Send_Nibble(byte val) const noexcept;
    inline void Send_Byte(byte val, byte mode) const noexcept;
    inline void Send_Command(byte val) const noexcept;
    inline void Send_Char(byte val) const noexcept;
    inline void Send_Write(byte val) const noexcept;
    inline void Init() noexcept;

 public:
    LCD_I2C(byte address, byte columns, byte rows, i2c_inst * I2C_instance = PICO_DEFAULT_I2C_INSTANCE,
            uint SDA_pin = PICO_DEFAULT_I2C_SDA_PIN, uint SCL_pin = PICO_DEFAULT_I2C_SCL_PIN) noexcept;
    void DisplayOn() noexcept;
    void DisplayOff() noexcept;
    void CursorOn() noexcept;
    void CursorOff() noexcept;
    void BlinkOn() noexcept;
    void BlinkOff() noexcept;
    void LeftToRight() noexcept;
    void RightToLeft() noexcept;
    void AutoscrollOn() noexcept;
    void AutoscrollOff() noexcept;
    void BacklightOn() noexcept;
    void BacklightOff() noexcept;
    void Clear() const noexcept;
    void Home() const noexcept;
    void ScrollDisplayLeft() const noexcept;
    void ScrollDisplayRight() const noexcept;
    void SetCursor(byte row, byte column) const noexcept;
    void PrintChar(byte character) const noexcept;
    void PrintString(std::string_view str) const noexcept;
    void PrintCustomChar(byte location) const noexcept;
    void CreateChar(byte location, byte const * char_map) const noexcept;
};

