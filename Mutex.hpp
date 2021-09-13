/*******************************************************************************
 * @file Mutex.hpp
 * @author Cristian Cristea
 * @date September 13, 2021
 * @brief Header file for the Mutex class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <pico/sync.h>

#include <memory>

class Mutex
{
 private:
    std::shared_ptr<mutex_t> mutex;

 public:

    /**
     * [Constructor]
     */
    Mutex() noexcept;

    /**
     * TODO
     */
    void Lock() const noexcept;

    /**
     * TODO
     */
    void Unlock() const noexcept;
};

