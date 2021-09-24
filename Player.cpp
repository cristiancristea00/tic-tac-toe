/*******************************************************************************
 * @file Symbol.cpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Source file for the Symbol class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Player.hpp"

using Utility::PlayerSymbol;

Player::Player(PlayerSymbol symbol, IPlayerStrategy * const strategy) noexcept : symbol(symbol), strategy(strategy) {}

auto Player::GetSymbol() const noexcept -> PlayerSymbol
{
    return symbol;
}

auto Player::GetStrategyName() const noexcept -> std::string_view
{
    return strategy->GetName();
}

void Player::SetSymbol(PlayerSymbol player_symbol) noexcept
{
    symbol = player_symbol;
}

auto Player::GetNextMove(Utility::Board const & current_board, Keypad * const keypad) noexcept -> Move
{
    return strategy->GetNextMove(current_board, keypad);
}