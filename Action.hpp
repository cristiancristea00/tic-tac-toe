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
    enum class Type
    {
        VALID,
        INVALID,
    };

    uint8_t row;
    uint8_t column;
    Type type;

    explicit Action(Type) noexcept;
    Action(uint8_t, uint8_t, Type) noexcept;
    friend bool operator==(Action const & lhs, Action const & rhs) noexcept;
};

