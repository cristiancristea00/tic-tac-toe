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

auto Action::GetRow() const noexcept -> int8_t
{
    return row;
}

auto Action::GetColumn() const noexcept -> int8_t
{
    return column;
}

auto operator==(Action const & lhs, Action const & rhs) noexcept -> bool
{
    return std::tie(lhs.row, lhs.column) == std::tie(rhs.row, rhs.column);
}

auto Action::Hash::operator()(Action const & action) const noexcept -> std::size_t
{
    return std::hash<int8_t>()(action.row) ^ std::hash<int8_t>()(action.column);
}
