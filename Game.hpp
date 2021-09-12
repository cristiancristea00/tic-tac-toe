/*******************************************************************************
 * @file Game.hpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Header file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#pragma once

#include <hardware/regs/rosc.h>
#include <pico/multicore.h>

#include "LCD_I2C.hpp"
#include "Action.hpp"
#include "TM1637.hpp"
#include "Keypad.hpp"

#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>
#include <array>

class Game final
{
 private:

    /**
     * Representation of game symbols
     */
    enum class BoardState : uint8_t
    {
        EMPTY,
        X,
        O,
        UNKNOWN
    };

    static constexpr uint8_t BOARD_SIZE = 3;

    using Board = std::array<std::array<BoardState, BOARD_SIZE>, BOARD_SIZE>;
    using Player = uint8_t;
    using Value = int8_t;

    static constexpr Player PLAYER_X = 'X';
    static constexpr Player PLAYER_0 = '0';
    static constexpr Player PLAYER_UNKNOWN = 'U';

    static constexpr LCD_I2C::byte LOCATION_LEFT = 0;
    static constexpr LCD_I2C::byte LOCATION_CENTER = 1;
    static constexpr LCD_I2C::byte LOCATION_RIGHT = 2;
    static constexpr LCD_I2C::byte LOCATION_X = 3;
    static constexpr LCD_I2C::byte LOCATION_0 = 4;
    static constexpr LCD_I2C::byte LOCATION_SPACE = 5;

    static constexpr LCD_I2C::byte TEXT_START_COLUMN = 8;

    /**
     * Replacement for +INFINITY
     */
    static constexpr Value VALUE_MAX = std::numeric_limits<Value>::max();

    /**
     * Replacement for -INFINITY
     */
    static constexpr Value VALUE_MIN = std::numeric_limits<Value>::min();

    Board game_board {};
    bool ai_turn = false;

    Value score_X {};
    Value score_0 {};

    std::unique_ptr<LCD_I2C> lcd;
    std::unique_ptr<TM1637> led_segments;
    std::unique_ptr<Keypad> keypad;

    /**
     * TODO
     */
    void Init_Second_Core() const noexcept;

    /**
     * Converts the player type to a board piece.
     *
     * @param player The player to be converted
     * @return The resulting piece
     */
    inline static auto Board_State_From_Player(Player player) noexcept -> BoardState;

    /**
     * Converts the board piece to a LCD screen custom character memory
     * location.
     *
     * @param board_state The piece to be converted
     * @return The resulting memory location
     */
    inline static auto LCD_Char_Location_From_Board_State(BoardState board_state) noexcept -> LCD_I2C::byte;

    /**
     * Resets the board to its initial state of emptiness.
     */
    inline void Reset_Board() noexcept;

    /**
     * Checks if the board is full with pieces.
     *
     * @param current_board The board to be checked
     * @return True or False
     */
    inline static auto Is_Board_Full(Board const & current_board) noexcept -> bool;

    /**
     * Checks if the player has a winning configuration on the board.
     *
     * @param current_player The current player
     * @param current_board The board to be checked
     * @return True or False
     */
    static auto Is_Winner(Player current_player, Board const & current_board) noexcept -> bool;

    /**
     * Computes the current player based on the number of pieces on the board.
     *
     * @param current_board The board to be analysed
     * @return The current player
     */
    static auto Get_Current_Player(Board const & current_board) noexcept -> Player;

    /**
     * Computes the available moves, i.e. the empty places on the board.
     *
     * @param current_board The board to be analysed
     * @return A vector of available moves
     */
    inline static auto Get_Actions(Board const & current_board) noexcept -> std::vector<Action>;

    /**
     * Get the winner of the current board configuration.
     *
     * @param current_board The board to be analysed
     * @return The winning player
     */
    inline static auto Get_Winner(Board const & current_board) noexcept -> Player;

    /**
     * Checks if the current board configuration is terminal.
     *
     * @param current_board The board to be checked
     * @return True or False
     */
    inline static auto Is_Terminal(Board const & current_board) noexcept -> bool;

    /**
     * Checks if a move can be made on the board.
     *
     * @param current_board The corresponding board
     * @param action The action to be checked
     * @return True or False
     */
    inline static auto Is_Valid_Action(Board const & current_board, Action const & action) noexcept -> bool;

    /**
     * Gets the score for a terminal board.
     *
     * @param current_board The board to be analysed
     * @return The board score
     */
    inline static auto Get_Board_Value(Board const & current_board) noexcept -> Value;

    /**
     * Computes the resulting board if a move is made on the current board.
     *
     * @param current_board The board to be analysed
     * @param action The move to be made
     * @return The resulting board
     */
    inline static auto Get_Result_Board(Board const & current_board, Action const & action, Player player) noexcept
    -> Game::Board;

    /**
     * Draws on the LCD the game board.
     */
    inline void Draw_Game() const noexcept;

    /**
     * Draws on the LCD the current board configuration.
     */
    inline void Draw_Board_State() const noexcept;

    /**
     * TODO
     *
     * @param winner
     */
    inline void Print_Winner_And_Update_Score(Player winner) noexcept;

    /**
     * TODO
     *
     * @param current_player
     */
    inline void Print_User_Info(Player current_player) const noexcept;

    /**
     * TODO
     */
    inline void Print_Computer_Info() const noexcept;

    /**
     * Main game logic
     */
    void Internal_Play() noexcept;

    /**
     * Refreshes the led display to display the score changes.
     */
    inline void Update_Scoreboard() const noexcept;

    /**
     * Increases the X player scores and updates the display.
     */
    inline void Increase_X_Score() noexcept;

    /**
     * Increases the 0 player scores and updates the display.
     */
    inline void Increase_0_Score() noexcept;

    /**
     * TODO
     */
    inline void Reset_Scoreboard() noexcept;

    /**
     * TODO
     *
     * @param key
     * @return
     */
    static auto Action_From_Key(Key key) noexcept -> Action;

    /**
     * TODO
     *
     * @param key
     * @return
     */
    static auto Player_From_Key(Key key) noexcept -> Player;

    /**
     * Chooses the game difficulty
     */
    inline void Choose_Difficulty() noexcept;

    /**
     * TODO
     *
     * @return
     */
    [[nodiscard]] inline auto Get_User() const noexcept -> Player;

    /**
     * TODO
     */
    [[noreturn]] static void Backlight_And_Reset_Runner() noexcept;

 public:

    /**
     * [Constructor] Defines the LCD custom symbols.
     *
     * @param lcd The LCD object used for display
     * @param led_segments The seven segment display used as a scoreboard
     * @param keypad The keypad used for input
     */
    Game(LCD_I2C * lcd, TM1637 * led_segments, Keypad * keypad) noexcept;

    /**
     * Main function that the user uses to start the game.
     */
    [[noreturn]] void Play() noexcept;

 private:

    class IGameStrategy
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

     public:

        /**
         * [Constructor]
         */
        IGameStrategy() noexcept;

        /**
         * TODO
         *
         * @return
         */
        auto GetRNG() noexcept -> std::mt19937 &;

        /**
         * Selects a move according to the current board configuration.
         *
         * @param current_board The board to be analysed
         * @return A move
         */
        [[nodiscard]] virtual auto GetNextMove(Board const & current_board) noexcept -> Action = 0;

        /**
         * [Destructor]
         */
        virtual ~IGameStrategy() noexcept = default;

        /**
         * [Copy constructor]
         */
        IGameStrategy(IGameStrategy const &) = default;

        /**
         * [Move constructor]
         */
        IGameStrategy(IGameStrategy &&) = default;

        /**
         * [Copy assigment operator]
         */
        auto operator=(IGameStrategy const &) -> IGameStrategy & = default;

        /**
         * [Move assigment operator]
         */
        auto operator=(IGameStrategy &&) -> IGameStrategy & = default;
    };

    class EasyStrategy final : public IGameStrategy
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
        [[nodiscard]] auto GetNextMove(Board const & current_board) noexcept -> Action final;

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

    class MediumStrategy final : public IGameStrategy
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
        [[nodiscard]] auto GetNextMove(Board const & current_board) noexcept -> Action final;

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

    class ImpossibleStrategy final : public IGameStrategy
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
        [[nodiscard]] auto Get_Min_Value(Board const & current_board, Value alpha, Value beta) const noexcept -> Value;

        /**
         * Helper function to get the maximum data possible used in the function
         * GetNextMove(Board const &).
         *
         * @param current_board The board to be analysed
         * @param alpha The alpha parameter
         * @param beta The beta parameter
         * @return The maximum data
         */
        [[nodiscard]] auto Get_Max_Value(Board const & current_board, Value alpha, Value beta) const noexcept -> Value;

        /**
         * TODO
         *
         * @param current_board
         * @return
         */
        [[nodiscard]] auto Get_Possible_Moves(Board const & current_board) const
        -> std::unordered_map<Action, Value, Action::Hash>;

     public:

        /**
         * [Constructor]
         */
        ImpossibleStrategy() noexcept = default;

        /**
         * Computes the best move for the current board configuration using the
         * depth-unlimited alpha–beta pruning minimax algorithm.
         *
         * @param current_board The board to be analysed
         * @return The best move
         */
        [[nodiscard]] auto GetNextMove(Board const & current_board) noexcept -> Action final;

        /**
         * [Destructor]
         */
        ~ImpossibleStrategy() noexcept final = default;

        /**
         * [Copy constructor]
         */
        ImpossibleStrategy(ImpossibleStrategy const &) = default;

        /**
         * [Move constructor]
         */
        ImpossibleStrategy(ImpossibleStrategy &&) = default;

        /**
         * [Copy assigment operator]
         */
        auto operator=(ImpossibleStrategy const &) -> ImpossibleStrategy & = default;

        /**
         * [Move assigment operator]
         */
        auto operator=(ImpossibleStrategy &&) -> ImpossibleStrategy & = default;
    };

    std::unique_ptr<IGameStrategy> game_strategy;

    /**
     * TODO
     *
     * @param key
     * @return
     */
    static auto Difficulty_From_Key(Key key) noexcept -> IGameStrategy *;
};