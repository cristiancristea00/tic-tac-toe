/*******************************************************************************
 * @file LockGuard.cpp
 * @author Cristian Cristea
 * @date September 13, 2021
 * @brief Source file for the LockGuard class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "LockGuard.hpp"

LockGuard::LockGuard(Mutex mtx) noexcept : mutex(std::move(mtx))
{
    mutex.Lock();
}

LockGuard::~LockGuard() noexcept
{
    mutex.Unlock();
}
