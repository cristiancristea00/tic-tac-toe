/*******************************************************************************
 * @file TM1637.cpp
 * @author Cristian Cristea
 * @date July 21, 2021
 * @brief Source file for the TM1637 class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "TM1637.hpp"

TM1637::TM1637(byte DIO, byte CLK, PIO pio) noexcept : DIO(DIO), CLK(CLK), pio(pio)
{
    gpio_pull_up(DIO);
    gpio_pull_up(CLK);
    pio_gpio_init(pio, DIO);
    pio_gpio_init(pio, CLK);

    state_machine = pio_claim_unused_sm(pio, true);
    auto offset = pio_add_program(pio, &tm1637_program);
    state_machine_config = tm1637_program_get_default_config(offset);

    sm_config_set_sideset_pins(&state_machine_config, CLK);
    value both_pins = (1u << CLK) | (1u << DIO);
    pio_sm_set_pins_with_mask(pio, state_machine, both_pins, both_pins);
    pio_sm_set_pindirs_with_mask(pio, state_machine, both_pins, both_pins);

    sm_config_set_out_pins(&state_machine_config, DIO, 1);
    sm_config_set_set_pins(&state_machine_config, DIO, 1);

    sm_config_set_out_shift(&state_machine_config, true, false, 32);

    Refresh_Frequency();

    pio_sm_init(pio, state_machine, offset, &state_machine_config);
    pio_sm_set_enabled(pio, state_machine, true);
}

inline void TM1637::Refresh_Frequency() const noexcept
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

inline void TM1637::Refresh_Display() const noexcept
{
    pio_sm_put_blocking(pio, state_machine, BRIGHTNESS_BASE + brightness);
}

void TM1637::Send_2_Bytes(value start_position, value data) const noexcept
{
    static value address;

    address = WRITE_ADDRESS + start_position;
    pio_sm_put_blocking(pio, state_machine, (data << 16) + (address << 8) + WRITE_MODE);

    Refresh_Display();
}

void TM1637::Send_4_Bytes(value start_position, value data) const noexcept
{
    static value address;
    static value data_1;
    static value data_2;

    address = WRITE_ADDRESS + start_position;
    data_1 = data & 0xFFFF;
    data_2 = data >> 16;
    pio_sm_put_blocking(pio, state_machine, (data_1 << 16) + (address << 8) + WRITE_MODE);
    pio_sm_put_blocking(pio, state_machine, data_2 << 16);

    Refresh_Display();
}

TM1637::value TM1637::Number_To_Segments(value number, value bit_mask) noexcept
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
            segments = temp_segments + (segments << 8);
        }
    }
    if (bit_mask)
    {
        segments &= bit_mask;
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
            segments = DIGIT_TO_SEGMENTS[0] + (segments << 8);
        }
        else
        {
            segments = segments << 8;
        }
    }

    return segments;
}

void TM1637::DisplayLeft(value number, bool leading_zeros) const noexcept
{
    Send_2_Bytes(0, Two_Digits_To_Segment(number, leading_zeros));
}

void TM1637::DisplayRight(value number, bool leading_zeros) const noexcept
{
    Send_2_Bytes(2, Two_Digits_To_Segment(number, leading_zeros));
}

void TM1637::DisplayColon() const noexcept
{
    Send_2_Bytes(0, COLON << 8);
}
