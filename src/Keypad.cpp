/*******************************************************************************
 * @file Keypad.cpp
 * @author Cristian Cristea
 * @date August 20, 2021
 * @brief Source file for the Keypad class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Keypad.hpp"

using Utility::PlayerSymbol;

Keypad::Keypad(array const & rows, array const & columns) noexcept : rows(rows), columns(columns)
{
    Init();
}

inline void Keypad::Init() noexcept
{
    std::for_each(rows.begin(), rows.end(), gpio_init);
    std::for_each(rows.begin(), rows.end(), [](byte row) {gpio_set_dir(row, GPIO_OUT);});
    std::for_each(rows.begin(), rows.end(), [](byte row) {gpio_put(row, LOW);});

    std::for_each(columns.begin(), columns.end(), gpio_init);
    std::for_each(columns.begin(), columns.end(), [](byte column) {gpio_set_dir(column, GPIO_IN);});
    std::for_each(columns.begin(), columns.end(), gpio_pull_down);

    std::reverse(rows.begin(), rows.end());
    std::reverse(columns.begin(), columns.end());
}

auto Keypad::Key_Poller() const noexcept -> Key
{
    static auto last_debounce_time = to_ms_since_boot(get_absolute_time());
    static constexpr auto DELAY_TIME = 150;

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
                sleep_us(1);
                if (gpio_get(columns[column]))
                {
                    gpio_put(rows[row], LOW);
                    return KEYS.at(row).at(column);
                }
                gpio_put(rows[row], LOW);
            }
        }
    }
    return Key::UNKNOWN;
}

auto Keypad::GetKeyFromPoller() const noexcept -> Key
{
    Key key {Key::UNKNOWN};

    do
    {
        key = Key_Poller();
    }
    while (key == Key::UNKNOWN);

    return key;
}

auto Keypad::GetPressedKey() noexcept -> Key
{
    return static_cast<Key>(multicore_fifo_pop_blocking());
}

auto Keypad::ActionFromKey(Key key) noexcept -> Move
{
    switch (key)
    {
        case Key::KEY1:
            return {0, 0};
        case Key::KEY2:
            return {0, 1};
        case Key::KEY3:
            return {0, 2};
        case Key::KEY5:
            return {1, 0};
        case Key::KEY6:
            return {1, 1};
        case Key::KEY7:
            return {1, 2};
        case Key::KEY9:
            return {2, 0};
        case Key::KEY10:
            return {2, 1};
        case Key::KEY11:
            return {2, 2};
        default:
            return {-1, -1};
    }
}

auto Keypad::PlayerFromKey(Key key) noexcept -> PlayerSymbol
{
    switch (key)
    {
        case Key::KEY15:
            return PlayerSymbol::X;
        case Key::KEY16:
            return PlayerSymbol::O;
        default:
            return PlayerSymbol::UNK;
    }
}

auto Keypad::DifficultyFromKey(Key key) noexcept -> IPlayerStrategy *
{
    switch (key)
    {
        case Key::KEY4:
            return new EasyStrategy;
        case Key::KEY8:
            return new MediumStrategy;
        case Key::KEY12:
            return new HardStrategy;
        default:
            return nullptr;
    }
}

auto Keypad::EnemyFromKey(Key key) noexcept -> std::string_view
{
    switch (key)
    {
        case Key::KEY15:
            return "HUMAN";
        case Key::KEY16:
            return "AI";
        default:
            return {};
    }
}

auto Keypad::AnswerFromKey(Key key) noexcept -> std::string_view
{
    switch (key)
    {
        case Key::KEY15:
            return "YES";
        case Key::KEY16:
            return "NO";
        default:
            return {};
    }
}

