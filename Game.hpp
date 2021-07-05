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

#include <fmt/core.h>

#include <array>
#include <vector>
#include <random>
#include <cstdint>
#include <algorithm>
#include <unordered_map>

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

 private:
    static BoardState BoardStateFromPlayer(Player) noexcept;
    static char CharFromBoardState(BoardState) noexcept;
    static bool Is_Board_Full(Board const &) noexcept;
    static bool Is_Winner(Player, Board const &) noexcept;
    static Player Get_Current_Player(Board const &) noexcept;
    static std::vector<Action> Get_Actions(Board const &) noexcept;
    static Player Get_Winner(Board const &) noexcept;
    static bool Is_Terminal(Board const &) noexcept;
    static Value Utility(Board const &) noexcept;
    static Board Get_Result_Board(Board const &, Action const &);
    Value Get_Min_Value(Board const &) const noexcept;
    Value Get_Max_Value(Board const &) const noexcept;
    Action Minimax(Board const &) const noexcept;
    void DrawBoard() const noexcept;

 public:
    Game() noexcept;
    void Play();
};

