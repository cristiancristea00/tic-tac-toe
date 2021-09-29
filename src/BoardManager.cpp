/*******************************************************************************
 * @file BoardManager.cpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Source file for the BoardManager class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "BoardManager.hpp"

using Utility::Board;
using Utility::BOARD_SIZE;
using Utility::PlayerSymbol;

BoardManager * BoardManager::instance = nullptr;

BoardManager::BoardManager() noexcept
{
    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            game_board[row][column] = PlayerSymbol::UNK;
        }
    }
}

auto BoardManager::Instance() noexcept -> BoardManager *
{
    if (instance == nullptr)
    {
        instance = new BoardManager;
    }
    return instance;
}

BoardManager::~BoardManager() noexcept
{
    delete instance;
}

auto BoardManager::GetGameBoard() noexcept -> Board &
{
    return game_board;
}

auto BoardManager::IsWinner(PlayerSymbol player, Board const & current_board) noexcept -> bool
{
    return (current_board[0][0] == player && current_board[0][1] == player && current_board[0][2] == player) ||
            (current_board[1][0] == player && current_board[1][1] == player && current_board[1][2] == player) ||
            (current_board[2][0] == player && current_board[2][1] == player && current_board[2][2] == player) ||
            (current_board[0][0] == player && current_board[1][0] == player && current_board[2][0] == player) ||
            (current_board[0][1] == player && current_board[1][1] == player && current_board[2][1] == player) ||
            (current_board[0][2] == player && current_board[1][2] == player && current_board[2][2] == player) ||
            (current_board[0][0] == player && current_board[1][1] == player && current_board[2][2] == player) ||
            (current_board[0][2] == player && current_board[1][1] == player && current_board[2][0] == player);
}

auto BoardManager::IsBoardFull(Board const & current_board) noexcept -> bool
{
    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] == PlayerSymbol::UNK)
            {
                return false;
            }
        }
    }
    return true;
}

auto BoardManager::GetCurrentPlayer(Board const & current_board) noexcept -> PlayerSymbol
{
    uint8_t moves = 0;
    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] != PlayerSymbol::UNK)
            {
                ++moves;
            }
        }
    }
    return (moves % 2 == 0) ? PlayerSymbol::X : PlayerSymbol::O;
}

auto BoardManager::GetActions(Board const & current_board) noexcept -> std::vector<Move>
{
    static std::vector<Move> actions;
    actions.reserve(BOARD_SIZE * BOARD_SIZE);

    actions.clear();

    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] == PlayerSymbol::UNK)
            {
                actions.emplace_back(row, column);
            }
        }
    }
    return actions;
}

auto BoardManager::GetWinner(Board const & current_board) noexcept -> PlayerSymbol
{
    if (IsWinner(PlayerSymbol::X, current_board))
    {
        return PlayerSymbol::X;
    }
    if (IsWinner(PlayerSymbol::O, current_board))
    {
        return PlayerSymbol::O;
    }
    return PlayerSymbol::UNK;
}

auto BoardManager::IsTerminal(Board const & current_board) noexcept -> bool
{
    return IsBoardFull(current_board) || IsWinner(PlayerSymbol::X, current_board)
            || IsWinner(PlayerSymbol::O, current_board);
}

auto BoardManager::IsValidAction(Board const & current_board, Move const & action) noexcept -> bool
{
    return action.GetRow() >= 0 && action.GetColumn() >= 0 && action.GetRow() < BOARD_SIZE
            && action.GetColumn() < BOARD_SIZE
            && current_board[action.GetRow()][action.GetColumn()] == PlayerSymbol::UNK;
}

auto BoardManager::GetBoardValue(Board const & current_board) noexcept -> Utility::Value
{
    if (IsWinner(PlayerSymbol::X, current_board))
    {
        return 1;
    }
    if (IsWinner(PlayerSymbol::O, current_board))
    {
        return -1;
    }
    return 0;
}

auto BoardManager::GetResultBoard(Board const & current_board, Move const & action, PlayerSymbol player)
noexcept -> Board
{
    auto action_board = current_board;
    action_board.at(action.GetRow()).at(action.GetColumn()) = player;
    return action_board;
}

void BoardManager::ResetBoard() noexcept
{
    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            game_board[row][column] = PlayerSymbol::UNK;
        }
    }
}
