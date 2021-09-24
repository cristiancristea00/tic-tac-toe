/*******************************************************************************
 * @file Utility.hpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Header file for the Utility namespace.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <array>

namespace Utility
{
/**
 * Representation of game and player symbols.
 */
enum class PlayerSymbol : uint8_t
{
    UNK,
    X,
    O
};

static constexpr uint8_t BOARD_SIZE = 3;
using Board = std::array<std::array<PlayerSymbol, BOARD_SIZE>, BOARD_SIZE>;

using Value = int8_t;
}  // namespace Utility

