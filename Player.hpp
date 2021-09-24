/*******************************************************************************
 * @file Symbol.hpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Header file for the Symbol class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include "IPlayerStrategy.hpp"
#include "Utility.hpp"

#include <memory>

class Player
{
 private:

    Utility::PlayerSymbol symbol {};

    std::unique_ptr<IPlayerStrategy> strategy;

 public:

    /**
     * [Constructor]
     *
     * @param symbol The player's symbol
     * @param strategy The player's game strategy
     */
    Player(Utility::PlayerSymbol symbol, IPlayerStrategy * strategy) noexcept;

    /**
     * Gets the player's symbol.
     *
     * @return The player's symbol
     */
    [[gnu::pure]][[nodiscard]] auto GetSymbol() const noexcept -> Utility::PlayerSymbol;

    /**
     * Gets the player's strategy's name.
     *
     * @return A string representation of the strategy's name
     */
    [[gnu::pure]][[nodiscard]] auto GetStrategyName() const noexcept -> std::string_view;

    /**
     * Sets the player's symbol.
     *
     * @param player_symbol The new symbol
     */
    void SetSymbol(Utility::PlayerSymbol player_symbol) noexcept;

    /**
     * Selects a move according to the current board configuration.
     *
     * @param current_board The board to be analysed
     * @return A move
     */
    [[nodiscard]] auto GetNextMove(Utility::Board const & current_board) noexcept -> Move;
};

