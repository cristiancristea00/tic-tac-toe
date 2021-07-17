/*******************************************************************************
 * @file Lcd.cpp
 * @author Cristian Cristea
 * @date July 08, 2021
 * @brief Source file for the LCD_I2C class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "LCD_I2C.hpp"

LCD_I2C::LCD_I2C(byte address, byte columns, byte rows, i2c_inst * I2C_instance, uint SDA_pin, uint SCL_pin) noexcept
        : address(address), columns(columns), rows(rows), backlight(NO_BACKLIGHT), I2C_instance(I2C_instance)
{
    i2c_init(I2C_instance, 100'000);
    gpio_set_function(SDA_pin, GPIO_FUNC_I2C);
    gpio_set_function(SCL_pin, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_pin);
    gpio_pull_up(SCL_pin);
    Init();
}

inline void LCD_I2C::I2C_Write_Byte(byte val) const noexcept // expanderWrite
{
    static byte data;

    data = val | backlight;
    i2c_write_blocking(I2C_instance, address, &data, 1, false);
}

void LCD_I2C::Pulse_Enable(byte val) const noexcept
{
    static constexpr uint16_t DELAY = 600;

    sleep_us(DELAY);
    I2C_Write_Byte(val | ENABLE);
    sleep_us(DELAY);
    I2C_Write_Byte(val & ~ENABLE);
    sleep_us(DELAY);
}

inline void LCD_I2C::Send_Nibble(byte val) const noexcept
{
    I2C_Write_Byte(val);
    Pulse_Enable(val);
}

inline void LCD_I2C::Send_Byte(byte val, byte mode) const noexcept
{
    byte high = val & 0xF0;
    byte low = (val << 4) & 0xF0;

    Send_Nibble(high | mode);
    Send_Nibble(low | mode);
}

inline void LCD_I2C::Send_Command(byte val) const noexcept
{
    Send_Byte(val, COMMAND);
}

inline void LCD_I2C::Send_Char(byte val) const noexcept
{
    Send_Byte(val, CHAR);
}

void LCD_I2C::Send_Write(byte val) const noexcept
{
    Send_Byte(val, REGISTER_SELECT);
}

void LCD_I2C::DisplayOn() noexcept
{
    display_control |= DISPLAY_ON;
    Send_Command(DISPLAY_CONTROL | display_control);
}

void LCD_I2C::DisplayOff() noexcept
{
    display_control &= ~DISPLAY_ON;
    Send_Command(DISPLAY_CONTROL | display_control);
}

void LCD_I2C::CursorOn() noexcept
{
    display_control |= CURSOR_ON;
    Send_Command(DISPLAY_CONTROL | display_control);
}

void LCD_I2C::CursorOff() noexcept
{
    display_control &= ~CURSOR_ON;
    Send_Command(DISPLAY_CONTROL | display_control);
}

void LCD_I2C::BlinkOn() noexcept
{
    display_control |= BLINK_ON;
    Send_Command(DISPLAY_CONTROL | display_control);
}

void LCD_I2C::BlinkOff() noexcept
{
    display_control &= ~BLINK_ON;
    Send_Command(DISPLAY_CONTROL | display_control);
}

void LCD_I2C::ScrollDisplayLeft() const noexcept
{
    Send_Command(CURSOR_SHIFT | DISPLAY_MOVE | MOVE_LEFT);
}

void LCD_I2C::ScrollDisplayRight() const noexcept
{
    Send_Command(CURSOR_SHIFT | DISPLAY_MOVE | MOVE_RIGHT);
}

void LCD_I2C::LeftToRight() noexcept
{
    display_mode |= ENTRY_LEFT;
    Send_Command(ENTRY_MODE_SET | display_mode);
}

void LCD_I2C::RightToLeft() noexcept
{
    display_mode &= ~ENTRY_LEFT;
    Send_Command(ENTRY_MODE_SET | display_mode);
}

void LCD_I2C::AutoscrollOn() noexcept
{
    display_mode |= ENTRY_SHIFT_INCREMENT;
    Send_Command(ENTRY_MODE_SET | display_mode);
}

void LCD_I2C::AutoscrollOff() noexcept
{
    display_mode &= ~ENTRY_SHIFT_INCREMENT;
    Send_Command(ENTRY_MODE_SET | display_mode);
}

void LCD_I2C::BacklightOn() noexcept
{
    backlight = BACKLIGHT;
    I2C_Write_Byte(backlight);
}

void LCD_I2C::BacklightOff() noexcept
{
    backlight = NO_BACKLIGHT;
    I2C_Write_Byte(backlight);
}

void LCD_I2C::Clear() const noexcept
{
    Send_Command(CLEAR_DISPLAY);
}

void LCD_I2C::Home() const noexcept
{
    Send_Command(RETURN_HOME);
}

inline void LCD_I2C::Init() noexcept
{
    display_mode = ENTRY_LEFT | ENTRY_SHIFT_DECREMENT;
    display_function = MODE_4_BIT | LINE_2 | DOTS_5x8;
    display_control = DISPLAY_ON | CURSOR_OFF | BLINK_OFF;

    Send_Command(0x03);
    Send_Command(0x03);
    Send_Command(0x03);
    Send_Command(0x02);

    Send_Command(ENTRY_MODE_SET | display_mode);
    Send_Command(FUNCTION_SET | display_function);
    DisplayOn();
    Clear();
    Home();
}

void LCD_I2C::SetCursor(byte row, byte column) const noexcept
{
    static constexpr byte row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    Send_Command(SET_DDRAM_ADDR | (row_offsets[row] + column));
}

void LCD_I2C::PrintChar(byte character) const noexcept
{
    Send_Char(character);
}

void LCD_I2C::PrintString(std::string_view str) const noexcept
{
    for (char const character : str)
    {
        PrintChar(character);
    }
}

void LCD_I2C::PrintCustomChar(byte location) const noexcept
{
    Send_Write(location);
}

void LCD_I2C::CreateChar(byte location, byte const * char_map) const noexcept
{
    location &= 0x7;
    Send_Command(SET_CGRAM_ADDR | (location << 3));
    for (int i = 0; i < 8; ++i)
    {
        Send_Write(char_map[i]);
    }
}
