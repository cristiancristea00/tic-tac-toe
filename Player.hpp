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
     * TODO
     *
     * @param symbol
     * @param strategy
     */
    Player(Utility::PlayerSymbol symbol, IPlayerStrategy * strategy) noexcept;

    /**
     * TODO
     *
     * @return
     */
    [[nodiscard]] auto GetSymbol() const noexcept -> Utility::PlayerSymbol;

    /**
     * TODO
     *
     * @param player_symbol
     */
    void SetSymbol(Utility::PlayerSymbol player_symbol) noexcept;

    /**
     * TODO
     *
     * @param current_board
     * @return
     */
    [[nodiscard]] auto GetNextMove(Utility::Board const & current_board, Keypad * keypad) noexcept -> Move;
};

