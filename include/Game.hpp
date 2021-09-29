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

#include "IPlayerStrategy.hpp"
#include "LCD_I2C.hpp"
#include "TM1637.hpp"
#include "Keypad.hpp"
#include "Player.hpp"
#include "Move.hpp"

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

    using byte = uint8_t;

    static constexpr byte LOCATION_LEFT = 0;
    static constexpr byte LOCATION_CENTER = 1;
    static constexpr byte LOCATION_RIGHT = 2;
    static constexpr byte LOCATION_X = 3;
    static constexpr byte LOCATION_0 = 4;
    static constexpr byte LOCATION_SPACE = 5;

    static constexpr byte TEXT_START_COLUMN = 8;

    std::unique_ptr<Player> first_player {std::make_unique<Player>(Utility::PlayerSymbol::UNK, new HumanStrategy)};
    std::unique_ptr<Player> second_player;

    std::pair<Utility::Value, Utility::Value> score {0, 0};

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
    inline static auto LCD_Char_Location_From_Player_Symbol(Utility::PlayerSymbol symbol) noexcept -> byte;

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
     * Informs the first player that it's their turn by printing on the LCD what symbol
     * is using.
     */
    inline void Print_First_Player_Info() const noexcept;

    /**
     * Informs the second player that it's their turn by printing on the LCD
     * what symbol is using if it's human or a message otherwise.
     */
    inline void Print_Second_Player_Info() const noexcept;

    /**
     * Main game logic
     */
    void Internal_Play() noexcept;

    /**
     * Refreshes the led display to display the score changes.
     */
    inline void Update_Scoreboard() const noexcept;

    /**
     * Increases the first player scores and updates the display.
     */
    inline void Increase_First_Player_Score() noexcept;

    /**
     * Increases the second player scores and updates the display.
     */
    inline void Increase_Second_Player_Score() noexcept;

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
     * Prompts the user to select between a Human or AI opponent.
     */
    void Choose_Enemy() noexcept;

    /**
     * Prompts the user to select if they want to continue playing with the same
     * opponent.
     */
    void Continue_After_Game() noexcept;

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