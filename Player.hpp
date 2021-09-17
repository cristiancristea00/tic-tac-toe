/*******************************************************************************
 * @file Symbol.hpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Header file for the Symbol class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include "Utility.hpp"

class Player
{
 private:

    Utility::PlayerSymbol symbol {};

 public:

    auto GetSymbol() const noexcept -> Utility::PlayerSymbol;
};

