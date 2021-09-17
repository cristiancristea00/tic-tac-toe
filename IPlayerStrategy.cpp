/*******************************************************************************
 * @file IPlayerStrategy.cpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Source file for the IPlayerStrategy class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "IPlayerStrategy.hpp"

using Utility::PlayerSymbol;
using Utility::Value;
using Utility::Board;

IPlayerStrategy::IPlayerStrategy() noexcept : random_number_generator(Get_Random_Seed()) {}

inline auto IPlayerStrategy::Get_Random_Seed() noexcept -> uint32_t
{
    static constexpr uint32_t FNV_OFFSET_BASIS = 0x811C9DC5;
    static constexpr uint32_t FNV_PRIME = 0x01000193;
    static constexpr size_t NO_OF_ROUNDS = 16;
    static constexpr size_t NO_OF_BYTES = 8;

    uint32_t random = FNV_OFFSET_BASIS;
    uint8_t next_byte = 0;
    auto * volatile random_reg = reinterpret_cast<uint32_t *>(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

    for (size_t i = 0; i < NO_OF_ROUNDS; i++)
    {
        for (size_t k = 0; k < NO_OF_BYTES; k++)
        {
            next_byte = (next_byte << 1) | (*random_reg & 1);
        }
        random ^= next_byte;
        random *= FNV_PRIME;
    }
    return random;
}

auto IPlayerStrategy::GetRNG() noexcept -> std::mt19937 &
{
    return random_number_generator;
}

auto EasyStrategy::GetNextMove(Board const & current_board) noexcept -> Move
{
    if (BoardManager::GetInstance()->IsTerminal(current_board))
    {
        return {};
    }

    auto actions = BoardManager::GetInstance()->GetActions(current_board);

    std::sample(actions.begin(), actions.end(), std::back_inserter(actions), 1, GetRNG());
    return actions.back();
}

auto MediumStrategy::GetNextMove(Board const & current_board) noexcept -> Move
{
    if (BoardManager::GetInstance()->IsTerminal(current_board))
    {
        return {};
    }

    auto actions = BoardManager::GetInstance()->GetActions(current_board);

    for (auto const & action: actions)
    {
        if (BoardManager::GetInstance()->
                IsWinner(PlayerSymbol::X, BoardManager::GetInstance()->
                GetResultBoard(current_board, action, PlayerSymbol::X)) ||
                BoardManager::GetInstance()->IsWinner(PlayerSymbol::O, BoardManager::GetInstance()->
                        GetResultBoard(current_board, action, PlayerSymbol::O)))
        {
            return action;
        }
    }

    std::sample(actions.begin(), actions.end(), std::back_inserter(actions), 1, GetRNG());
    return actions.back();
}

auto HardStrategy::Get_Min_Value(Board const & current_board, Value alpha, Value beta) const noexcept -> Value
{
    if (BoardManager::GetInstance()->IsTerminal(current_board))
    {
        return BoardManager::GetInstance()->GetBoardValue(current_board);
    }

    Value value = VALUE_MAX;

    auto current_actions = BoardManager::GetInstance()->GetActions(current_board);
    for (Move const & action: current_actions)
    {
        value = std::min(value, Get_Max_Value(BoardManager::GetInstance()->
                GetResultBoard(current_board, action, BoardManager::GetInstance()->
                GetCurrentPlayer(current_board)), alpha, beta));
        beta = std::min(beta, value);
        if (value <= alpha)
        {
            return value;
        }
    }
    return value;
}

auto HardStrategy::Get_Max_Value(Board const & current_board, Value alpha, Value beta) const noexcept -> Value
{
    if (BoardManager::GetInstance()->IsTerminal(current_board))
    {
        return BoardManager::GetInstance()->GetBoardValue(current_board);
    }

    Value value = VALUE_MIN;

    auto current_actions = BoardManager::GetInstance()->GetActions(current_board);
    for (Move const & action: current_actions)
    {
        value = std::max(value, Get_Min_Value(BoardManager::GetInstance()->
                GetResultBoard(current_board, action, BoardManager::GetInstance()->
                GetCurrentPlayer(current_board)), alpha, beta));
        alpha = std::max(alpha, value);
        if (value >= beta)
        {
            return value;
        }
    }
    return value;
}

auto HardStrategy::Get_Possible_Moves(Board const & current_board) const
-> std::unordered_map<Move, Value, Move::Hash>
{
    auto actions = BoardManager::GetInstance()->GetActions(current_board);

    std::unordered_map<Move, Value, Move::Hash> possible_moves;
    if (BoardManager::GetInstance()->GetCurrentPlayer(current_board) == PlayerSymbol::X)
    {
        Value max_value = VALUE_MIN;
        for (Move const & action: actions)
        {
            possible_moves.emplace(action, Get_Min_Value(BoardManager::GetInstance()->
                    GetResultBoard(current_board, action, BoardManager::GetInstance()->
                    GetCurrentPlayer(current_board)), VALUE_MIN, VALUE_MAX));
            if (possible_moves[action] > max_value)
            {
                max_value = possible_moves[action];
            }
        }
        std::erase_if(possible_moves, [=](auto const & elem)
        {
            return elem.second != max_value;
        });
    }
    else
    {
        Value min_value = VALUE_MAX;
        for (Move const & action: actions)
        {
            possible_moves.emplace(action, Get_Max_Value(BoardManager::GetInstance()->
                    GetResultBoard(current_board, action, BoardManager::GetInstance()->
                    GetCurrentPlayer(current_board)), VALUE_MIN, VALUE_MAX));
            if (possible_moves[action] < min_value)
            {
                min_value = possible_moves[action];
            }
        }
        std::erase_if(possible_moves, [=](auto const & elem)
        {
            return elem.second != min_value;
        });
    }

    return possible_moves;
}

auto HardStrategy::GetNextMove(Board const & current_board) noexcept -> Move
{
    if (BoardManager::GetInstance()->IsTerminal(current_board))
    {
        return {};
    }

    auto possible_moves = Get_Possible_Moves(current_board);

    std::vector<std::pair<Move, Value>> result {possible_moves.begin(), possible_moves.end()};
    for (auto const &[ACTION, VALUE]: result)
    {
        if (BoardManager::GetInstance()->
                IsWinner(BoardManager::GetInstance()->GetCurrentPlayer(current_board),
                         BoardManager::GetInstance()->GetResultBoard(current_board, ACTION,
                                                                     BoardManager::GetInstance()->
                                                                             GetCurrentPlayer(current_board))))
        {
            return ACTION;
        }
    }
    std::sample(result.begin(), result.end(), std::back_inserter(result), 1, GetRNG());
    return result.back().first;
}
