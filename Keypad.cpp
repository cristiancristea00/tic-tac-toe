/*******************************************************************************
 * @file Keypad.cpp
 * @author Cristian Cristea
 * @date August 20, 2021
 * @brief Source file for the Keypad class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Keypad.hpp"

Keypad::Keypad(array const & rows, array const & columns) noexcept : rows(rows), columns(columns)
{
    std::for_each(columns.begin(), columns.end(), gpio_init);
    std::for_each(rows.begin(), rows.end(), gpio_init);
    std::for_each(rows.begin(), rows.end(), [](byte row) {gpio_set_dir(row, GPIO_OUT);});
    std::for_each(columns.begin(), columns.end(), gpio_pull_down);
    std::for_each(columns.begin(), columns.end(), [](byte column) {gpio_set_dir(column, GPIO_IN);});
    std::for_each(rows.begin(), rows.end(), [](byte row) {gpio_put(row, LOW);});
    Init();
}

inline void Keypad::Init() noexcept
{
    std::reverse(rows.begin(), rows.end());
    std::reverse(columns.begin(), columns.end());
}

[[nodiscard]] __attribute__((optimize("O0")))
auto Keypad::Poll_Keys() const noexcept -> Key
{
    static auto last_debounce_time = to_ms_since_boot(get_absolute_time());
    static constexpr auto DELAY_TIME = 100;

    if ((to_ms_since_boot(get_absolute_time()) - last_debounce_time) > DELAY_TIME)
    {
        last_debounce_time = to_ms_since_boot(get_absolute_time());

        #pragma GCC unroll 4
        for (size_t row = 0; row < KEYPAD_SIZE; ++row)
        {
            #pragma GCC unroll 4
            for (size_t column = 0; column < KEYPAD_SIZE; ++column)
            {
                gpio_put(rows[row], HIGH);
                if (gpio_get(columns[column]))
                {
                    return KEYS.at(row).at(column);
                }
                gpio_put(rows[row], LOW);
            }
        }
    }
    return Key::UNKOWN;
}

[[nodiscard]] auto Keypad::GetPressedKey() const noexcept -> Key
{
    Key key {};

    do
    {
        key = Poll_Keys();
    }
    while (key == Key::UNKOWN);

    return key;
}
