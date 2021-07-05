/*******************************************************************************
 * @file Action.cpp
 * @author Cristian Cristea
 * @date July 05, 2021
 * @brief Source file for the Action class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include <tuple>
#include "Action.hpp"

Action::Action(uint8_t row, uint8_t column, Type type) noexcept : row(row), column(column), type(type) {}

Action::Action() noexcept : row(0), column(0), type(Type::INVALID) {}

bool operator==(Action const & lhs, Action const & rhs) noexcept
{
    return std::tie(lhs.row, lhs.column, lhs.type) == std::tie(rhs.row, rhs.column, rhs.type);
}
