/*******************************************************************************
 * @file Action.cpp
 * @author Cristian Cristea
 * @date July 05, 2021
 * @brief Source file for the Action class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Action.hpp"

Action::Action(int8_t row, int8_t column) noexcept : row(row), column(column) {}

bool operator==(Action const & lhs, Action const & rhs) noexcept
{
    return std::tie(lhs.row, lhs.column) == std::tie(rhs.row, rhs.column);
}

size_t Action::Hash::operator()(Action const & action) const noexcept
{
    return std::hash<int8_t>()(action.row) ^ std::hash<int8_t>()(action.column);
}
