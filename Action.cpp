/*******************************************************************************
 * @file Action.cpp
 * @author Cristian Cristea
 * @date July 05, 2021
 * @brief Source file for the Action class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Action.hpp"

Action::Action(uint8_t row, uint8_t column) noexcept : row(row), column(column) {}

bool operator==(Action const & lhs, Action const & rhs) noexcept
{
    return std::tie(lhs.row, lhs.column) == std::tie(rhs.row, rhs.column);
}
