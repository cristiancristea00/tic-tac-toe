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
#include <tuple>

class Action
{
 public:
    struct Hash
    {
        size_t operator()(Action const & action) const noexcept;
    };

    int8_t row {};
    int8_t column {};

    Action() noexcept = default;
    Action(int8_t row, int8_t column) noexcept;
    friend bool operator==(Action const & lhs, Action const & rhs) noexcept;
};

