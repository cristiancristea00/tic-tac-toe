/*******************************************************************************
 * @file Game.hpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Header file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <hardware/regs/rosc.h>
#include <pico/multicore.h>

#include "LCD_I2C.hpp"
#include "Player.hpp"
#include "Move.hpp"
#include "TM1637.hpp"
#include "Keypad.hpp"
#include "IPlayerStrategy.hpp"

#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>
#include <array>

class Game final
{
 private:

    static constexpr LCD_I2C::byte LOCATION_LEFT = 0;
    static constexpr LCD_I2C::byte LOCATION_CENTER = 1;
    static constexpr LCD_I2C::byte LOCATION_RIGHT = 2;
    static constexpr LCD_I2C::byte LOCATION_X = 3;
    static constexpr LCD_I2C::byte LOCATION_0 = 4;
    static constexpr LCD_I2C::byte LOCATION_SPACE = 5;

    static constexpr LCD_I2C::byte TEXT_START_COLUMN = 8;

    std::unique_ptr<Player> first_player {std::make_unique<Player>(Utility::PlayerSymbol::UNK, new HumanStrategy)};
    std::unique_ptr<Player> second_player;

    Utility::Value score_X {};
    Utility::Value score_0 {};

    std::unique_ptr<LCD_I2C> lcd;
    std::unique_ptr<TM1637> led_segments;
    std::unique_ptr<Keypad> keypad;

    /**
     * Starts the key poller on the second core.
     */
    void Init_Second_Core() const noexcept;

    /**
     * Converts the board piece to a LCD screen custom character memory
     * location.
     *
     * @param symbol The piece to be converted
     * @return The resulting memory location
     */
    inline static auto LCD_Char_Location_From_Player_Symbol(Utility::PlayerSymbol symbol) noexcept -> LCD_I2C::byte;

    /**
     * Draws on the LCD the game board.
     */
    inline void Draw_Game() const noexcept;

    /**
     * Draws on the LCD the current board configuration.
     */
    inline void Draw_Board_State() const noexcept;

    /**
     * Prints the winner on the LCD and updates the scoreboard.
     *
     * @param winner The game winner
     */
    inline void Print_Winner_And_Update_Score(Utility::PlayerSymbol winner) noexcept;

    /**
     * Informs the user that it's their turn by printing on the LCD what symbol
     * is using.
     *
     * @param current_player The current player
     */
    inline void Print_User_Turn_Info(Utility::PlayerSymbol current_player) const noexcept;

    /**
     * Prints on the LCD that it's the computer turn.
     */
    inline void Print_Computer_Info() const noexcept;

    /**
     * Main game logic
     */
    void Internal_Play() noexcept;

    /**
     * Refreshes the led display to display the score changes.
     */
    inline void Update_Scoreboard() const noexcept;

    /**
     * Increases the X player scores and updates the display.
     */
    inline void Increase_X_Score() noexcept;

    /**
     * Increases the 0 player scores and updates the display.
     */
    inline void Increase_0_Score() noexcept;

    /**
     * Resets the scoreboard to its initial values.
     */
    inline void Reset_Scoreboard() noexcept;

    /**
     * Chooses the game difficulty
     */
    inline auto Get_Difficulty() noexcept -> IPlayerStrategy *;

    /**
     * Prints on the LCD the current game difficulty.
     *
     * @param diff Game difficulty
     */
    inline void Print_Difficulty(std::string_view diff) noexcept;

    /**
     * Prints on the LCD screen the choice between game symbols and waits for a
     * key to be pressed to select the symbol.
     *
     * @return The chosen player
     */
    [[nodiscard]] inline auto Get_User() const noexcept -> Utility::PlayerSymbol;

    /**
     * Key poller that runs on the second core.
     */
    [[noreturn]] static void Key_Poller_Runner() noexcept;

    /**
     * TODO
     */
    void Choose_Oponent() noexcept;

 public:

    /**
     * [Constructor] Defines the LCD custom symbols.
     *
     * @param lcd The LCD object used for display
     * @param led_segments The seven segment display used as a scoreboard
     * @param keypad The keypad used for input
     */
    Game(LCD_I2C * lcd, TM1637 * led_segments, Keypad * keypad) noexcept;

    /**
     * Main function that the user uses to start the game.
     */
    [[noreturn]] void Play() noexcept;
};