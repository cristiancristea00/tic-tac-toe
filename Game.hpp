/*******************************************************************************
 * @file Game.hpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Header file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

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

    using Player = uint8_t;
    using Value = int8_t;
    using Action = std::pair<uint8_t, uint8_t>;
    using Board = std::array<std::array<BoardState, 3>, 3>;

    Board game_board;

    static constexpr uint8_t BOARD_SIZE = 3;
    static constexpr Player PLAYER_X = 'X';
    static constexpr Player PLAYER_0 = '0';
    static constexpr Player PLAYER_UNKNOWN = 'U';

 private:
    static BoardState BoardStateFromPlayer(Player) noexcept;
    static bool Is_Board_Full(Board const &) noexcept;
    static bool Is_Winner(Player, Board const &) noexcept;
    static Player Get_Current_Player(Board const &) noexcept;
    static std::vector<Action> Get_Actions(Board const &) noexcept;
    static Player Get_Winner(Board const &) noexcept;
    static bool Is_Terminal(Board const &) noexcept;
    static Value Utility(Board const &) noexcept;
    static Board Get_Result_Board(Board const &, Action const &) noexcept;
    Value Get_Min_Value(Board const &) const noexcept;
    Value Get_Max_Value(Board const &) const noexcept;
    Action Minimax(Board const &) const noexcept;

 public:
    Game() noexcept = default;
};

