/*******************************************************************************
 * @file BoardManager.hpp
 * @author Cristian Cristea
 * @date September 17, 2021
 * @brief Header file for the BoardManager class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include "Utility.hpp"
#include "Move.hpp"

#include <cstdint>
#include <vector>
#include <array>

class BoardManager final
{
 private:

    static BoardManager * instance;

    BoardManager() noexcept;

    Utility::Board game_board {};

 public:

    /**
     * Singleton method to retrieve the instance
     *
     * @return
     */
    static auto Instance() noexcept -> BoardManager *;

    /**
     * [Destructor]
     */
    ~BoardManager() noexcept;

    /**
     * [Copy constructor]
     */
    BoardManager(BoardManager const &) = delete;

    /**
     * [Move constructor]
     */
    BoardManager(BoardManager &&) = delete;

    /**
     * [Copy assigment operator]
     */
    auto operator=(BoardManager const &) -> BoardManager & = delete;

    /**
     * [Move assigment operator]
     */
    auto operator=(BoardManager &&) -> BoardManager & = delete;

    [[gnu::pure]] [[nodiscard]] auto GetGameBoard() noexcept -> Utility::Board &;

    /**
     * Checks if the player has a winning configuration on the board.
     *
     * @param player The current player
     * @param current_board The board to be checked
     * @return True or False
     */
    [[gnu::pure]] auto IsWinner(Utility::PlayerSymbol player, Utility::Board const & current_board)
    noexcept -> bool;

    /**
     * Checks if the board is full with pieces.
     *
     * @param current_board The board to be checked
     * @return True or False
     */
    [[gnu::pure]] auto IsBoardFull(Utility::Board const & current_board) noexcept -> bool;

    /**
     * Computes the current player based on the number of pieces on the board.
     *
     * @param current_board The board to be analysed
     * @return The current player
     */
    [[gnu::pure]] auto GetCurrentPlayer(Utility::Board const & current_board) noexcept -> Utility::PlayerSymbol;

    /**
     * Computes the available moves, i.e. the empty places on the board.
     *
     * @param current_board The board to be analysed
     * @return A vector of available moves
     */
    auto GetActions(Utility::Board const & current_board) noexcept -> std::vector<Move>;

    /**
     * Get the winner of the current board configuration.
     *
     * @param current_board The board to be analysed
     * @return The winning player
     */
    [[gnu::pure]] auto GetWinner(Utility::Board const & current_board) noexcept -> Utility::PlayerSymbol;

    /**
     * Checks if the current board configuration is terminal.
     *
     * @param current_board The board to be checked
     * @return True or False
     */
    [[gnu::pure]] auto IsTerminal(Utility::Board const & current_board) noexcept -> bool;

    /**
     * Checks if a move can be made on the board.
     *
     * @param current_board The corresponding board
     * @param action The action to be checked
     * @return True or False
     */
    [[gnu::pure]] auto IsValidAction(Utility::Board const & current_board, Move const & action) noexcept -> bool;

    /**
     * Gets the score for a terminal board.
     *
     * @param current_board The board to be analysed
     * @return The board score
     */
    [[gnu::pure]] auto GetBoardValue(Utility::Board const & current_board) noexcept -> Utility::Value;

    /**
     * Computes the resulting board if a move is made on the current board.
     *
     * @param current_board The board to be analysed
     * @param action The move to be made
     * @return The resulting board
     */
    [[gnu::pure]] auto GetResultBoard(Utility::Board const & current_board, Move const & action,
                        Utility::PlayerSymbol player) noexcept -> Utility::Board;

    /**
     * Resets the board to its initial state of emptiness.
     */
    void ResetBoard() noexcept;
};

