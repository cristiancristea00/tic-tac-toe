/*******************************************************************************
 * @file Action.hpp
 * @author Cristian Cristea
 * @date July 05, 2021
 * @brief Header file for the Action class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <memory>

class Action
{
 private:
    int8_t row {};
    int8_t column {};

 public:

    /**
     * TODO
     */
    Action() noexcept = default;

    /**
     * TODO
     *
     * @param row
     * @param column
     */
    Action(int8_t row, int8_t column) noexcept;

    /**
     * TODO
     *
     * @return
     */
    [[nodiscard]] auto GetRow() const noexcept -> int8_t;

    /**
     * TODO
     *
     * @return
     */
    [[nodiscard]] auto GetColumn() const noexcept -> int8_t;

    /**
     * TODO
     *
     * @param lhs
     * @param rhs
     * @return
     */
    friend auto operator==(Action const & lhs, Action const & rhs) noexcept -> bool;

    struct Hash
    {
        auto operator()(Action const & action) const noexcept -> std::size_t;
    };
};

