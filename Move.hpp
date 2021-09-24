/*******************************************************************************
 * @file Move.hpp
 * @author Cristian Cristea
 * @date July 05, 2021
 * @brief Header file for the Move class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <memory>

class Move
{
 private:
    int8_t row {};
    int8_t column {};

 public:

    /**
     * [Constructor]
     */
    Move() noexcept = default;

    /**
     * [Constructor] Initialises the action with a specific position.
     *
     * @param row The vertical position (Y axis)
     * @param column The horizontal position (X axis)
     */
    Move(int8_t row, int8_t column) noexcept;

    /**
     * Getter function for the row member.
     *
     * @return Row value
     */
    [[gnu::pure]][[nodiscard]] auto GetRow() const noexcept -> int8_t;

    /**
     * Getter function for the column member.
     *
     * @return Column value
     */
    [[gnu::pure]][[nodiscard]] auto GetColumn() const noexcept -> int8_t;

    /**
     * [Equality operator] Checks if the Move objects are the same.
     *
     * @param lhs Left-hand side operand
     * @param rhs Right-hand side operand
     * @return True if the objects are the same, false otherwise
     */
    friend auto operator==(Move const & lhs, Move const & rhs) noexcept -> bool;

    struct Hash
    {
        [[gnu::pure]] auto operator()(Move const & action) const noexcept -> std::size_t;
    };
};

