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
    static constexpr size_t PULL_THRESHOLD = 32;

    state_machine = static_cast<byte>(pio_claim_unused_sm(pio, true));
    auto offset = pio_add_program(pio, &TM1637_program);
    state_machine_config = TM1637_program_get_default_config(offset);

    sm_config_set_sideset_pins(&state_machine_config, CLK);
    uint32_t both_pins = (1 << CLK) | (1 << DIO);
    pio_sm_set_pins_with_mask(pio, state_machine, both_pins, both_pins);
    pio_sm_set_pindirs_with_mask(pio, state_machine, both_pins, both_pins);

    sm_config_set_out_pins(&state_machine_config, DIO, 1);
    sm_config_set_set_pins(&state_machine_config, DIO, 1);

    sm_config_set_out_shift(&state_machine_config, true, false, PULL_THRESHOLD);

    Set_Clock_Divider();

    pio_sm_init(pio, state_machine, offset, &state_machine_config);
    pio_sm_set_enabled(pio, state_machine, true);
}

inline void TM1637::Set_Clock_Divider() noexcept
{
    static constexpr size_t FREQUENCY = 45'000;
    static constexpr size_t MAX_DIVIDER_VALUE = 65'536;

    data system_frequency = clock_get_hz(clk_sys);
    float divider = static_cast<float>(system_frequency) / FREQUENCY;
    if (divider > MAX_DIVIDER_VALUE)
    {
        divider = MAX_DIVIDER_VALUE;
    }
    else if (divider < 1)
    {
        divider = 1;
    }
    sm_config_set_clkdiv(&state_machine_config, divider);
}

void TM1637::Send_4_Bytes(data value) const noexcept
{
    static constexpr size_t BIT_MASK = 0xFF'FF;
    static constexpr size_t SHIFT_POSITIONS = 16;

    static data data_1;
    static data data_2;

    data_1 = value & BIT_MASK;
    data_2 = value >> SHIFT_POSITIONS;

    pio_sm_put_blocking(pio, state_machine, (data_1 << (2 * BYTE_SIZE)) +
            (WRITE_ADDRESS << BYTE_SIZE) + WRITE_MODE);
    pio_sm_put_blocking(pio, state_machine, data_2 << (2 * BYTE_SIZE));

    pio_sm_put_blocking(pio, state_machine, BRIGHTNESS_BASE + brightness);
}

auto TM1637::Number_To_Segments(data number, bool hex, data bitmask) noexcept -> TM1637::data
{
    auto base = hex ? BASE_HEX : BASE_TEN;
    auto max_one_digit = base - 1;

    static data segments;
    static data temp_segments;

    if (std::cmp_less_equal(number, max_one_digit))
    {
        segments = DIGIT_TO_SEGMENTS.at(number);
    }
    else
    {
        while (number != 0)
        {
            temp_segments = DIGIT_TO_SEGMENTS.at(number % base);
            number /= base;
            segments = temp_segments + (segments << BYTE_SIZE);
        }
    }
    if (bitmask != 0)
    {
        segments &= bitmask;
    }
    return segments;
}

auto TM1637::Two_Digits_To_Segment(data number, bool hex, bool leading_zeros) noexcept -> TM1637::data
{
    static constexpr size_t BIT_MASK = 0xFF'FF;

    data segments = Number_To_Segments(number, hex, BIT_MASK);

    auto base = hex ? BASE_HEX : BASE_TEN;

    auto num_div = number / base;

    if (num_div == 0)
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

void TM1637::Internal_Display(int16_t number, bool hex, bool leading_zeros) noexcept
{
    static constexpr byte DASH = 0x40;

    bool is_positive {true};
    uint16_t number_copy {0};

    int16_t base = hex ? BASE_HEX : BASE_TEN;

    if (number >= 0)
    {
        is_positive = true;
        number_copy = static_cast<uint16_t>(number);
    }
    else
    {
        is_positive = false;
        number_copy = static_cast<uint16_t>(-number);
    }

    byte length = 0;

    while (number_copy != 0)
    {
        ++length;
        number_copy /= base;
    }

    if (length > 3 + static_cast<byte>(is_positive))
    {
        length = 3 + static_cast<byte>(is_positive);
    }

    auto segments = Number_To_Segments(static_cast<data>(number), hex);
    byte start_position = 0;
    if (leading_zeros && length < MAX_DIGITS)
    {
        if (is_positive)
        {
            for (size_t i = length; i < MAX_DIGITS; ++i)
            {
                segments = (segments << BYTE_SIZE) + DIGIT_TO_SEGMENTS[0];
            }
        }
        else
        {
            for (size_t i = length; i < MAX_DIGITS - 1; ++i)
            {
                segments = (segments << BYTE_SIZE) + DIGIT_TO_SEGMENTS[0];
            }
            segments = (segments << BYTE_SIZE) + DASH;
            ++length;
        }
    }
    else
    {
        if (!is_positive)
        {
            segments = (segments << BYTE_SIZE) + DASH;
            ++length;
        }
        start_position = MAX_DIGITS - length;
    }

    current_segments = segments << (start_position * BYTE_SIZE);
    if (is_colon)
    {
        ColonOn();
    }
    Send_4_Bytes(current_segments);
}

void TM1637::Internal_Display_Left(data number, bool hex, bool leading_zeros) noexcept
{
    static constexpr size_t LEFT_BYTE_MASK = 0xFF'FF'00'00;

    current_segments = (current_segments & LEFT_BYTE_MASK) + Two_Digits_To_Segment(number, hex, leading_zeros);
    if (is_colon)
    {
        ColonOn();
    }
    Send_4_Bytes(current_segments);
}

void TM1637::Internal_Display_Right(data number, bool hex, bool leading_zeros) noexcept
{
    static constexpr size_t RIGHT_BYTE_MASK = 0x00'00'FF'FF;

    current_segments = (current_segments & RIGHT_BYTE_MASK) +
            (Two_Digits_To_Segment(number, hex, leading_zeros) << (2 * BYTE_SIZE));
    if (is_colon)
    {
        ColonOn();
    }
    Send_4_Bytes(current_segments);
}

void TM1637::Display(int16_t number, bool leading_zeros) noexcept
{
    Internal_Display(number, false, leading_zeros);
}

void TM1637::DisplayHex(int16_t number, bool leading_zeros) noexcept
{
    Internal_Display(number, true, leading_zeros);
}

void TM1637::DisplayLeft(data number, bool leading_zeros) noexcept
{
    Internal_Display_Left(number, false, leading_zeros);
}

void TM1637::DisplayLeftHex(data number, bool leading_zeros) noexcept
{
    Internal_Display_Left(number, true, leading_zeros);
}

void TM1637::DisplayRight(data number, bool leading_zeros) noexcept
{
    Internal_Display_Right(number, false, leading_zeros);
}

void TM1637::DisplayRightHex(data number, bool leading_zeros) noexcept
{
    Internal_Display_Right(number, true, leading_zeros);
}

void TM1637::SetBrightness(byte brightness_level) noexcept
{
    brightness = std::min(brightness_level, MAX_BRIGHTNESS);
    Send_4_Bytes(current_segments);
}

void TM1637::ColonOn() noexcept
{
    static constexpr size_t CONTROL_DISPLAY = 0x80'00;

    is_colon = true;
    current_segments |= CONTROL_DISPLAY;
    Send_4_Bytes(current_segments);
}

void TM1637::ColonOff() noexcept
{
    static constexpr size_t CONTROL_DISPLAY = 0x80'00;

    is_colon = false;
    current_segments &= ~CONTROL_DISPLAY;
    Send_4_Bytes(current_segments);
}

void TM1637::Clear() const noexcept
{
    static constexpr size_t CONTROL_DISPLAY = 0x80;
    static constexpr size_t WRITE_ADDRESS_AND_MODE = 0xC0'40;

    pio_sm_put_blocking(pio, state_machine, CONTROL_DISPLAY);
    pio_sm_put_blocking(pio, state_machine, WRITE_ADDRESS_AND_MODE);
    pio_sm_put_blocking(pio, state_machine, 0);
}
