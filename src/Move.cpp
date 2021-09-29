/*******************************************************************************
 * @file Move.cpp
 * @author Cristian Cristea
 * @date July 05, 2021
 * @brief Source file for the Move class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Move.hpp"

Move::Move(int8_t row, int8_t column) noexcept : row(row), column(column) {}

auto Move::GetRow() const noexcept -> int8_t
{
    return row;
}

auto Move::GetColumn() const noexcept -> int8_t
{
    return column;
}

[[gnu::pure]] auto operator==(Move const & lhs, Move const & rhs) noexcept -> bool
{
    return std::tie(lhs.row, lhs.column) == std::tie(rhs.row, rhs.column);
}

auto Move::Hash::operator()(Move const & action) const noexcept -> std::size_t
{
    return std::hash<int8_t>()(action.row) ^ std::hash<int8_t>()(action.column);
}
