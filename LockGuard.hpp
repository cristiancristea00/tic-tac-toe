/*******************************************************************************
 * @file LockGuard.hpp
 * @author Cristian Cristea
 * @date September 13, 2021
 * @brief Header file for the LockGuard class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include "Mutex.hpp"

class LockGuard
{
 private:
    Mutex mutex;

 public:

    /**
     * [Constructor]
     *
     * @param mtx Mutex to be managed
     */
    explicit LockGuard(Mutex mtx) noexcept;

    /**
     * [Destructor]
     */
    ~LockGuard() noexcept;

    /**
     * [Copy constructor]
     */
    LockGuard(LockGuard const &) = default;

    /**
     * [Move constructor]
     */
    LockGuard(LockGuard &&) = default;

    /**
     * [Copy assigment operator]
     */
    auto operator=(LockGuard const &) -> LockGuard & = default;

    /**
     * [Move assigment operator]
     */
    auto operator=(LockGuard &&) -> LockGuard & = default;
};

