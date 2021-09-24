/*******************************************************************************
 * @file IPlayerStrategy.hpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Header file for the IPlayerStrategy class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <hardware/regs/addressmap.h>
#include <hardware/regs/rosc.h>

#include "BoardManager.hpp"
#include "Utility.hpp"

#include <unordered_map>
#include <random>

class IPlayerStrategy
{
 private:

    std::mt19937 random_number_generator;

    /**
     * Random function found on Google that does the job, using something
     * called Fowler–Noll–Vo hash function. I have no idea what it does.
     *
     * @return Random seed generated from the current board parameters
     */
    inline static auto Get_Random_Seed() noexcept -> uint32_t;

 protected:

    /**
     * Replacement for +INFINITY
     */
    static constexpr auto VALUE_MAX = std::numeric_limits<Utility::Value>::max();

    /**
     * Replacement for -INFINITY
     */
    static constexpr auto VALUE_MIN = std::numeric_limits<Utility::Value>::min();

 public:

    /**
     * [Constructor]
     */
    IPlayerStrategy() noexcept;

    /**
     * Getter for the Random Number Generator (RNG).
     *
     * @return The RNG
     */
    auto GetRNG() noexcept -> std::mt19937 &;

    /**
     * Selects a move according to the current board configuration.
     *
     * @param current_board The board to be analysed
     * @return A move
     */
    [[nodiscard]] virtual auto GetNextMove(Utility::Board const & current_board) noexcept -> Move = 0;

    /**
     * Gets the strategy's name.
     *
     * @return A string representation of the strategy's name
     */
    [[gnu::pure]][[nodiscard]] virtual auto GetName() const noexcept -> std::string_view = 0;

    /**
     * [Destructor]
     */
    virtual ~IPlayerStrategy() noexcept = default;

    /**
     * [Copy constructor]
     */
    IPlayerStrategy(IPlayerStrategy const &) = default;

    /**
     * [Move constructor]
     */
    IPlayerStrategy(IPlayerStrategy &&) = default;

    /**
     * [Copy assigment operator]
     */
    auto operator=(IPlayerStrategy const &) -> IPlayerStrategy & = default;

    /**
     * [Move assigment operator]
     */
    auto operator=(IPlayerStrategy &&) -> IPlayerStrategy & = default;
};

class EasyStrategy final : public IPlayerStrategy
{
 public:

    /**
     * [Constructor]
     */
    EasyStrategy() noexcept = default;

    /**
     * Selects a random move from the available ones.
     *
     * @param current_board The board to be analysed
     * @return A random move
     */
    [[nodiscard]] auto GetNextMove(Utility::Board const & current_board) noexcept -> Move final;

    /**
     * Gets the strategy's name.
     *
     * @return A string representation of the strategy's name
     */
    [[gnu::pure]][[nodiscard]] auto GetName() const noexcept -> std::string_view final;

    /**
     * [Destructor]
     */
    ~EasyStrategy() noexcept final = default;

    /**
     * [Copy constructor]
     */
    EasyStrategy(EasyStrategy const &) = default;

    /**
     * [Move constructor]
     */
    EasyStrategy(EasyStrategy &&) = default;

    /**
     * [Copy assigment operator]
     */
    auto operator=(EasyStrategy const &) -> EasyStrategy & = default;

    /**
     * [Move assigment operator]
     */
    auto operator=(EasyStrategy &&) -> EasyStrategy & = default;
};

class MediumStrategy final : public IPlayerStrategy
{
 public:

    /**
     * [Constructor]
     */
    MediumStrategy() noexcept = default;

    /**
     * Selects a random move from the available ones or a winning/blocking
     * move if one is available.
     *
     * @param current_board The board to be analysed
     * @return A somewhat good move
     */
    [[nodiscard]] auto GetNextMove(Utility::Board const & current_board) noexcept -> Move final;

    /**
     * Gets the strategy's name.
     *
     * @return A string representation of the strategy's name
     */
    [[gnu::pure]][[nodiscard]] auto GetName() const noexcept -> std::string_view final;

    /**
     * [Destructor]
     */
    ~MediumStrategy() noexcept final = default;

    /**
     * [Copy constructor]
     */
    MediumStrategy(MediumStrategy const &) = default;

    /**
     * [Move constructor]
     */
    MediumStrategy(MediumStrategy &&) = default;

    /**
     * [Copy assigment operator]
     */
    auto operator=(MediumStrategy const &) -> MediumStrategy & = default;

    /**
     * [Move assigment operator]
     */
    auto operator=(MediumStrategy &&) -> MediumStrategy & = default;
};

class HardStrategy final : public IPlayerStrategy
{
 private:

    /**
     * Helper function to get the minimum data possible used in the function
     * GetNextMove(Board const &).
     *
     * @param current_board The board to be analysed
     * @param alpha The alpha parameter
     * @param beta The beta parameter
     * @return The minimum data
     */
    [[nodiscard]] auto Get_Min_Value(Utility::Board const & current_board,
                                     Utility::Value alpha, Utility::Value beta) const noexcept -> Utility::Value;

    /**
     * Helper function to get the maximum data possible used in the function
     * GetNextMove(Board const &).
     *
     * @param current_board The board to be analysed
     * @param alpha The alpha parameter
     * @param beta The beta parameter
     * @return The maximum data
     */
    [[nodiscard]] auto Get_Max_Value(Utility::Board const & current_board,
                                     Utility::Value alpha, Utility::Value beta) const noexcept -> Utility::Value;

    /**
     * Helper function to get all the possible moves for the current board
     * configuration
     *
     * @param current_board The board to be analysed
     * @return All the possible moves
     */
    [[nodiscard]] auto Get_Possible_Moves(Utility::Board const & current_board) const
    -> std::unordered_map<Move, Utility::Value, Move::Hash>;

 public:

    /**
     * [Constructor]
     */
    HardStrategy() noexcept = default;

    /**
     * Computes the best move for the current board configuration using the
     * depth-unlimited alpha–beta pruning minimax algorithm.
     *
     * @param current_board The board to be analysed
     * @return The best move
     */
    [[nodiscard]] auto GetNextMove(Utility::Board const & current_board) noexcept -> Move final;

    /**
     * Gets the strategy's name.
     *
     * @return A string representation of the strategy's name
     */
    [[gnu::pure]][[nodiscard]] auto GetName() const noexcept -> std::string_view final;

    /**
     * [Destructor]
     */
    ~HardStrategy() noexcept final = default;

    /**
     * [Copy constructor]
     */
    HardStrategy(HardStrategy const &) = default;

    /**
     * [Move constructor]
     */
    HardStrategy(HardStrategy &&) = default;

    /**
     * [Copy assigment operator]
     */
    auto operator=(HardStrategy const &) -> HardStrategy & = default;

    /**
     * [Move assigment operator]
     */
    auto operator=(HardStrategy &&) -> HardStrategy & = default;
};

class HumanStrategy final : public IPlayerStrategy
{
 public:

    /**
     * [Constructor]
     */
    HumanStrategy() noexcept = default;

    /**
     * Returns a move based on the pressed key.
     *
     * @param current_board The board to be analysed
     * @return The input move
     */
    [[nodiscard]] auto GetNextMove(Utility::Board const & current_board) noexcept -> Move final;

    /**
     * Gets the strategy's name.
     *
     * @return A string representation of the strategy's name
     */
    [[gnu::pure]][[nodiscard]] auto GetName() const noexcept -> std::string_view final;

    /**
     * [Destructor]
     */
    ~HumanStrategy() noexcept final = default;

    /**
     * [Copy constructor]
     */
    HumanStrategy(HumanStrategy const &) = default;

    /**
     * [Move constructor]
     */
    HumanStrategy(HumanStrategy &&) = default;

    /**
     * [Copy assigment operator]
     */
    auto operator=(HumanStrategy const &) -> HumanStrategy & = default;

    /**
     * [Move assigment operator]
     */
    auto operator=(HumanStrategy &&) -> HumanStrategy & = default;
};