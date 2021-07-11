/*******************************************************************************
 * @file Game.hpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Header file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

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

    Board game_board {};
    Player user = PLAYER_UNKNOWN;
    bool ai_turn = false;

    LCD_I2C * lcd = nullptr;

 private:
    inline static BoardState BoardStateFromPlayer(Player player) noexcept;
    inline static char CharFromBoardState(BoardState board_state) noexcept;
    inline static LCD_I2C::byte LCDCharLocationFromBoardState(BoardState board_state) noexcept;
    inline void Reset_Board() noexcept;
    inline static bool Is_Board_Full(Board const & current_board) noexcept;
    static bool Is_Winner(Player current_player, Board const & current_board) noexcept;
    static Player Get_Current_Player(Board const & current_board) noexcept;
    inline static std::vector<Action> Get_Actions(Board const & current_board) noexcept;
    inline static Player Get_Winner(Board const & current_board) noexcept;
    inline static bool Is_Terminal(Board const & current_board) noexcept;
    inline static Value Utility(Board const & current_board) noexcept;
    inline static bool Is_Valid_Action(Board const & current_board, Action const & action) noexcept;
    inline static Board Get_Result_Board(Board const & current_board, Action const & action) noexcept;
    [[nodiscard]] Value Get_Min_Value(Board const & current_board) const noexcept;
    [[nodiscard]] Value Get_Max_Value(Board const & current_board) const noexcept;
    [[nodiscard]] inline Action Minimax(Board const & current_board) const noexcept;
    void DrawBoard() const noexcept;
    void Internal_Play() noexcept;

 public:
    explicit Game(LCD_I2C * lcd) noexcept;
    [[noreturn]] void Play() noexcept;
};
