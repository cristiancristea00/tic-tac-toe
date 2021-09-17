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

auto Player::GetSymbol() const noexcept -> PlayerSymbol
{
    return symbol;
}