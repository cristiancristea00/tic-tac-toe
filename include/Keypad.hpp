/*******************************************************************************
 * @file Keypad.hpp
 * @author Cristian Cristea
 * @date August 20, 2021
 * @brief Header file for the Keypad class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <pico/time.h>

#include "IPlayerStrategy.hpp"
#include "Utility.hpp"
#include "Move.hpp"

#include <cstdint>
#include <array>

enum class Key
{
    KEY1, KEY2, KEY3, KEY4,
    KEY5, KEY6, KEY7, KEY8,
    KEY9, KEY10, KEY11, KEY12,
    KEY13, KEY14, KEY15, KEY16,
    UNKNOWN
};

class Keypad final
{
 private:
    using byte = uint8_t;

    static constexpr byte KEYPAD_SIZE = 4;

 public:
    using array = std::array<byte, KEYPAD_SIZE>;

 private:
    static constexpr bool LOW = false;
    static constexpr bool HIGH = true;

    array rows;
    array columns;

    static constexpr std::array<std::array<Key, KEYPAD_SIZE>, KEYPAD_SIZE>
            KEYS {{{Key::KEY1, Key::KEY2, Key::KEY3, Key::KEY4},
                   {Key::KEY5, Key::KEY6, Key::KEY7, Key::KEY8},
                   {Key::KEY9, Key::KEY10, Key::KEY11, Key::KEY12},
                   {Key::KEY13, Key::KEY14, Key::KEY15, Key::KEY16}}};

    /**
     * Initialises the arrays that contains the keypad pins.
     */
    inline void Init() noexcept;

    /**
     * Polls the keypad to check if a button was pressed. It uses a software
     * debounce method. If no key was pressed it returns unknown.
     *
     * @return The pressed key or unknown
     */
    [[nodiscard]] auto Key_Poller() const noexcept -> Key;

 public:

    /**
     * [Constructor]
     *
     * @param rows The rows pins
     * @param columns The columns pins
     */
    Keypad(array const & rows, array const & columns) noexcept;

    /**
     * Blocks the execution until a key is pressed.
     *
     * @return The pressed key
     */
    [[nodiscard]] auto GetKeyFromPoller() const noexcept -> Key;

    /**
     * Gets the pressed key from the intercore FIFO.
     *
     * @return The pressed key
     */
    [[nodiscard]] static auto GetPressedKey() noexcept -> Key;

    /**
     * Chooses a move based on the pressed key.
     *
     * @param key The pressed key
     * @return The corresponding game move
     */
    static auto ActionFromKey(Key key) noexcept -> Move;

    /**
     * Chooses a player based on the pressed key.
     *
     * @param key The pressed key
     * @return The corresponding game player
     */
    static auto PlayerFromKey(Key key) noexcept -> Utility::PlayerSymbol;

    /**
     * Chooses a move based on the pressed key.
     *
     * @param key The pressed key
     * @return The difficulty
     */
    static auto DifficultyFromKey(Key key) noexcept -> IPlayerStrategy *;

    /**
     * Chooses an opponent based on the pressed key.
     *
     * @param key The pressed key
     * @return A string version of the opponent
     */
    static auto EnemyFromKey(Key key) noexcept -> std::string_view;

    /**
     * Chooses an answer based on the pressed key.
     *
     * @param key The pressed key
     * @return A string version of the answer
     */
    static auto AnswerFromKey(Key key) noexcept -> std::string_view;
};

