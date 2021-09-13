/*******************************************************************************
 * @file Mutex.cpp
 * @author Cristian Cristea
 * @date September 13, 2021
 * @brief Source file for the Mutex class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Mutex.hpp"

Mutex::Mutex() noexcept : mutex(new mutex_t)
{
    mutex_init(mutex.get());
}

void Mutex::Lock() const noexcept
{
    mutex_enter_blocking(mutex.get());
}

void Mutex::Unlock() const noexcept
{
    mutex_exit(mutex.get());
}
