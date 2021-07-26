/*******************************************************************************
 * @file TM1637.cpp
 * @author Cristian Cristea
 * @date July 21, 2021
 * @brief Source file for the TM1637 class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "TM1637.hpp"

TM1637::TM1637(byte DIO, byte CLK, PIO pio) noexcept : pio(pio)
{
    gpio_pull_up(DIO);
    gpio_pull_up(CLK);
    pio_gpio_init(pio, DIO);
    pio_gpio_init(pio, CLK);
    Init(DIO, CLK);
}

inline void TM1637::Init(byte DIO, byte CLK) noexcept
{
    state_machine = pio_claim_unused_sm(pio, true);
    auto offset = pio_add_program(pio, &TM1637_program);
    state_machine_config = TM1637_program_get_default_config(offset);

    sm_config_set_sideset_pins(&state_machine_config, CLK);
    auto both_pins = (1u << CLK) | (1u << DIO);
    pio_sm_set_pins_with_mask(pio, state_machine, both_pins, both_pins);
    pio_sm_set_pindirs_with_mask(pio, state_machine, both_pins, both_pins);

    sm_config_set_out_pins(&state_machine_config, DIO, 1);
    sm_config_set_set_pins(&state_machine_config, DIO, 1);

    sm_config_set_out_shift(&state_machine_config, true, false, 32);

    Set_Clock_Divider();

    pio_sm_init(pio, state_machine, offset, &state_machine_config);
    pio_sm_set_enabled(pio, state_machine, true);
}

inline void TM1637::Set_Clock_Divider() const noexcept
{
    value system_frequency = clock_get_hz(clk_sys);
    float divider = static_cast<float>(system_frequency) / 45000;
    if (divider > 65536)
    {
        divider = 65536;
    }
    else if (divider < 1)
    {
        divider = 1;
    }
    sm_config_set_clkdiv(const_cast<pio_sm_config *>(&state_machine_config), divider);
}

void TM1637::Send_4_Bytes(value data) const noexcept
{
    static value data_1;
    static value data_2;

    data_1 = data & 0xFFFF;
    data_2 = data >> 16;
    pio_sm_put_blocking(pio, state_machine, (data_1 << (2 * BYTE_SIZE)) +
            (WRITE_ADDRESS << BYTE_SIZE) + WRITE_MODE);
    pio_sm_put_blocking(pio, state_machine, data_2 << (2 * BYTE_SIZE));

    pio_sm_put_blocking(pio, state_machine, BRIGHTNESS_BASE + brightness);
}

TM1637::value TM1637::Number_To_Segments(value number, value bitmask) noexcept
{
    static value segments;
    static value temp_segments;

    if (number <= 9)
    {
        segments = DIGIT_TO_SEGMENTS[number];
    }
    else
    {
        while (number != 0)
        {
            temp_segments = DIGIT_TO_SEGMENTS[number % 10];
            number /= 10;
            segments = temp_segments + (segments << BYTE_SIZE);
        }
    }
    if (bitmask)
    {
        segments &= bitmask;
    }
    return segments;
}

unsigned long TM1637::Two_Digits_To_Segment(value number, bool leading_zeros) noexcept
{
    value segments = Number_To_Segments(number, 0xFFFF);

    auto num_div = number / 10;

    if (!num_div)
    {
        if (leading_zeros)
        {
            segments = DIGIT_TO_SEGMENTS[0] + (segments << BYTE_SIZE);
        }
        else
        {
            segments = segments << BYTE_SIZE;
        }
    }

    return segments;
}

void TM1637::SetBrightness(byte brightness_level) noexcept
{
    brightness = std::min(brightness_level, MAX_BRIGHTNESS);
    Send_4_Bytes(current_segments);
}

void TM1637::Display(int16_t number, bool leading_zeros) noexcept
{
    bool is_positive;

    if (number >= 0)
    {
        is_positive = true;
    }
    else
    {
        is_positive = false;
        number = -number;
    }

    byte length = 0;
    auto number_copy = number;

    while (number_copy)
    {
        ++length;
        number_copy /= 10;
    }

    if (length > 3 + is_positive)
    {
        length = 3 + is_positive;
    }

    auto segments = Number_To_Segments(number);
    byte start_position = 0;
    if (leading_zeros && length < MAX_DIGITS)
    {
        if (is_positive)
        {
            for (int i = length; i < MAX_DIGITS; ++i)
            {
                segments = (segments << BYTE_SIZE) + DIGIT_TO_SEGMENTS[0];
            }
        }
        else
        {
            segments = (segments << BYTE_SIZE) + 0x40;
            ++length;
            for (int i = length; i < MAX_DIGITS - 1; ++i)
            {
                segments = (segments << BYTE_SIZE) + DIGIT_TO_SEGMENTS[0];
            }
        }
    }
    else
    {
        if (!is_positive)
        {
            segments = (segments << BYTE_SIZE) + 0x40;
            ++length;
        }
        start_position = MAX_DIGITS - length;
    }

    current_segments = segments << (start_position * BYTE_SIZE);
    Send_4_Bytes(current_segments);
}

void TM1637::DisplayLeft(value number, bool leading_zeros) noexcept
{
    current_segments = (current_segments & 0xFFFF0000) + Two_Digits_To_Segment(number, leading_zeros);
    if (is_colon)
    {
        ColonOn();
    }
    Send_4_Bytes(current_segments);
}

void TM1637::DisplayRight(value number, bool leading_zeros) noexcept
{
    current_segments = (current_segments & 0x0000FFFF) +
            (Two_Digits_To_Segment(number, leading_zeros) << (2 * BYTE_SIZE));
    if (is_colon)
    {
        ColonOn();
    }
    Send_4_Bytes(current_segments);
}

void TM1637::ColonOn() noexcept
{
    is_colon = true;
    current_segments |= 0x8000;
    Send_4_Bytes(current_segments);
}

void TM1637::ColonOff() noexcept
{
    is_colon = false;
    current_segments &= ~0x8000;
    Send_4_Bytes(current_segments);
}

void TM1637::Clear() const noexcept
{
    pio_sm_put_blocking(pio, state_machine, 0x80);
    pio_sm_put_blocking(pio, state_machine, 0xC040);
    pio_sm_put_blocking(pio, state_machine, 0x0);
}
