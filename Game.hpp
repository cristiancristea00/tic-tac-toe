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

#include "Action.hpp"
#include "LCD_I2C.hpp"

#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>
#include <array>

class Game
{
 private:

    /**
     * Representation of game symbols
     */
    enum class BoardState
    {
        EMPTY,
        X,
        O,
        UNKNOWN
    };

    static constexpr uint8_t BOARD_SIZE = 3;

    using Player = uint8_t;
    using Value = int8_t;
    using Board = std::array<std::array<BoardState, BOARD_SIZE>, BOARD_SIZE>;

    static constexpr Player PLAYER_X = 'X';
    static constexpr Player PLAYER_0 = '0';
    static constexpr Player PLAYER_UNKNOWN = 'U';

    /**
     * Replacement for +INFINITY
     */
    static constexpr Value VALUE_MAX = std::numeric_limits<Value>::max();

    /**
     * Replacement for -INFINITY
     */
    static constexpr Value VALUE_MIN = std::numeric_limits<Value>::min();

    Board game_board {};
    Player user = PLAYER_UNKNOWN;
    bool ai_turn = false;

    LCD_I2C * lcd = nullptr;

 private:

    /**
     * Random function found on Google that does the job, using something called
     * Fowler–Noll–Vo hash function. I have no idea what it does.
     *
     * @return Random seed generated from the current board parameters
     */
    inline static uint32_t Get_Random_Seed() noexcept;

    /**
     * Converts the player type to a board piece.
     *
     * @param player The player to be converted
     * @return The resulting piece
     */

    inline static BoardState Board_State_From_Player(Player player) noexcept;

    /**
     * Converts the board piece to a character, used for debug printing.
     *
     * @param board_state The piece to be converted
     * @return The resulting character
     */
    inline static char Char_From_Board_State(BoardState board_state) noexcept;

    /**
     * Converts the board piece to a LCD screen custom character memory
     * location.
     *
     * @param board_state The piece to be converted
     * @return The resulting memory location
     */
    inline static LCD_I2C::byte LCD_Char_Location_From_Board_State(BoardState board_state) noexcept;

    /**
     * Resets the board to its initial state of emptiness.
     */
    inline void Reset_Board() noexcept;

    /**
     * Checks if the board is full with pieces.
     *
     * @param current_board The board to be checked
     * @return True or False
     */
    inline static bool Is_Board_Full(Board const & current_board) noexcept;

    /**
     * Checks if the player has a winning configuration on the board.
     *
     * @param current_player The current player
     * @param current_board The board to be checked
     * @return True or False
     */
    static bool Is_Winner(Player current_player, Board const & current_board) noexcept;

    /**
     * Computes the current player based on the number of pieces on the board.
     *
     * @param current_board The board to be analysed
     * @return The current player
     */
    static Player Get_Current_Player(Board const & current_board) noexcept;

    /**
     * Computes the available moves, i.e. the empty places on the board.
     *
     * @param current_board The board to be analysed
     * @return A vector of available moves
     */
    inline static std::vector<Action> Get_Actions(Board const & current_board) noexcept;

    /**
     * Get the winner of the current board configuration.
     *
     * @param current_board The board to be analysed
     * @return The winning player
     */
    inline static Player Get_Winner(Board const & current_board) noexcept;

    /**
     * Checks if the current board configuration is terminal.
     *
     * @param current_board The board to be checked
     * @return True or False
     */
    inline static bool Is_Terminal(Board const & current_board) noexcept;

    /**
     * Checks if a move can be made on the board.
     *
     * @param current_board The corresponding board
     * @param action The action to be checked
     * @return True or False
     */
    inline static bool Is_Valid_Action(Board const & current_board, Action const & action) noexcept;

    /**
     * Gets the score for a terminal board.
     *
     * @param current_board The board to be analysed
     * @return The board score
     */
    inline static Value Get_Board_Value(Board const & current_board) noexcept;

    /**
     * Computes the resulting board if a move is made on the current board.
     *
     * @param current_board The board to be analysed
     * @param action The move to be made
     * @return The resulting board
     */
    inline static Board Get_Result_Board(Board const & current_board, Action const & action) noexcept;

    /**
     * Computes the best move for the current board configuration using the
     * alpha–beta pruning minimax algorithm.
     *
     * @param current_board The board to be analysed
     * @return The best move
     */
    [[nodiscard]] inline Action Get_Best_Move(Board const & current_board) const noexcept;

    /**
     * Helper function to get the minimum value possible used in the function
     * Get_Best_Move(Board const &).
     *
     * @param current_board The board to be analysed
     * @param alpha The alpha parameter
     * @param beta The beta parameter
     * @return The minimum value
     */
    [[nodiscard]] Game::Value Get_Min_Value(Board const & current_board, Value alpha, Value beta) const noexcept;

    /**
     * Helper function to get the maximum value possible used in the function
     * Get_Best_Move(Board const &).
     *
     * @param current_board The board to be analysed
     * @param alpha The alpha parameter
     * @param beta The beta parameter
     * @return The maximum value
     */
    [[nodiscard]] Game::Value Get_Max_Value(Board const & current_board, Value alpha, Value beta) const noexcept;

    /**
     * Draws on the LCD the game board.
     */
    inline void Draw_Game() const noexcept;

    /**
     * Draws on the LCD the current board configuration.
     */
    void Draw_Board_State() const noexcept;

    /**
     * Main game logic
     */
    void Internal_Play() noexcept;

 public:

    /**
     * [Constructor] Defines the LCD custom symbols.
     * @param lcd The LCD object used for display
     */
    explicit Game(LCD_I2C * lcd) noexcept;

    /**
     * Main function that the user uses to start the game.
     */
    [[noreturn]] void Play() noexcept;
};
