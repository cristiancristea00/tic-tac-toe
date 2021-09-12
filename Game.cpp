/*******************************************************************************
 * @file Game.cpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Source file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Game.hpp"

Game::Game(LCD_I2C * lcd, TM1637 * led_segments, Keypad * keypad) noexcept
        : lcd(lcd), led_segments(led_segments), keypad(keypad)
{
    static constexpr size_t NO_CUSTOM_SYMBOLS = 6;

    static constexpr std::array<std::array<LCD_I2C::byte, LCD_I2C::CUSTOM_SYMBOL_SIZE>, NO_CUSTOM_SYMBOLS>
            CUSTOM_SYMBOLS {{{0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07}, /* LEFT */
                             {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F}, /* CENTER */
                             {0x1C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1C}, /* RIGHT */
                             {0x00, 0x11, 0x0A, 0x04, 0x04, 0x0A, 0x11, 0x00}, /* X */
                             {0x00, 0x0E, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00}, /* 0 */
                             {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  /* ' ' */}};

    # pragma GCC unroll 6
    for (size_t location = 0; location < NO_CUSTOM_SYMBOLS; ++location)
    {
        lcd->CreateCustomChar(location, CUSTOM_SYMBOLS.at(location));
    }

    Init_Second_Core();
}

void Game::Init_Second_Core() const noexcept
{
    multicore_launch_core1(Backlight_And_Reset_Runner);
    multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(keypad.get()));
    multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(lcd.get()));
    multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(this));
}

inline auto Game::Board_State_From_Player(Player player) noexcept -> Game::BoardState
{
    if (player == PLAYER_X)
    {
        return BoardState::X;
    }
    if (player == PLAYER_0)
    {
        return BoardState::O;
    }
    return BoardState::UNKNOWN;
}

auto Game::LCD_Char_Location_From_Board_State(BoardState board_state) noexcept -> LCD_I2C::byte
{
    switch (board_state)
    {
        case BoardState::X:
            return LOCATION_X;
        case BoardState::O:
            return LOCATION_0;
        default:
            return LOCATION_SPACE;
    }
}

void Game::Reset_Board() noexcept
{
    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            game_board[row][column] = BoardState::EMPTY;
        }
    }
}

inline auto Game::Is_Board_Full(Board const & current_board) noexcept -> bool
{
    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] == BoardState::EMPTY)
            {
                return false;
            }
        }
    }
    return true;
}

auto Game::Is_Winner(Player current_player, Board const & current_board) noexcept -> bool
{
    static BoardState symbol;

    symbol = Board_State_From_Player(current_player);
    return (current_board[0][0] == symbol && current_board[0][1] == symbol && current_board[0][2] == symbol) ||
            (current_board[1][0] == symbol && current_board[1][1] == symbol && current_board[1][2] == symbol) ||
            (current_board[2][0] == symbol && current_board[2][1] == symbol && current_board[2][2] == symbol) ||
            (current_board[0][0] == symbol && current_board[1][0] == symbol && current_board[2][0] == symbol) ||
            (current_board[0][1] == symbol && current_board[1][1] == symbol && current_board[2][1] == symbol) ||
            (current_board[0][2] == symbol && current_board[1][2] == symbol && current_board[2][2] == symbol) ||
            (current_board[0][0] == symbol && current_board[1][1] == symbol && current_board[2][2] == symbol) ||
            (current_board[0][2] == symbol && current_board[1][1] == symbol && current_board[2][0] == symbol);
}

auto Game::Get_Current_Player(Board const & current_board) noexcept -> Game::Player
{
    uint8_t moves = 0;
    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] != BoardState::EMPTY)
            {
                ++moves;
            }
        }
    }
    return (moves % 2 == 0) ? PLAYER_X : PLAYER_0;
}

inline auto Game::Get_Actions(Board const & current_board) noexcept -> std::vector<Action>
{
    static std::vector<Action> actions;
    actions.reserve(BOARD_SIZE * BOARD_SIZE);

    actions.clear();

    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        #pragma GCC unroll 3
        for (size_t column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] == BoardState::EMPTY)
            {
                actions.emplace_back(row, column);
            }
        }
    }
    return actions;
}

inline auto Game::Get_Winner(Board const & current_board) noexcept -> Game::Player
{
    if (Is_Winner(PLAYER_X, current_board))
    {
        return PLAYER_X;
    }
    if (Is_Winner(PLAYER_0, current_board))
    {
        return PLAYER_0;
    }
    return PLAYER_UNKNOWN;
}

inline auto Game::Is_Terminal(Board const & current_board) noexcept -> bool
{
    return Is_Board_Full(current_board) || Is_Winner(PLAYER_X, current_board) || Is_Winner(PLAYER_0, current_board);
}

inline auto Game::Is_Valid_Action(Board const & current_board, Action const & action) noexcept -> bool
{
    return action.GetRow() >= 0 && action.GetColumn() >= 0 && action.GetRow() < BOARD_SIZE
            && action.GetColumn() < BOARD_SIZE
            && current_board[action.GetRow()][action.GetColumn()] == BoardState::EMPTY;
}

inline auto Game::Get_Board_Value(Board const & current_board) noexcept -> Game::Value
{
    if (Is_Winner(PLAYER_X, current_board))
    {
        return 1;
    }
    if (Is_Winner(PLAYER_0, current_board))
    {
        return -1;
    }
    return 0;
}

inline auto Game::Get_Result_Board(Board const & current_board,
                                   Action const & action,
                                   Player player) noexcept -> Game::Board
{
    auto action_board = current_board;
    action_board.at(action.GetRow()).at(action.GetColumn()) = Board_State_From_Player(player);
    return action_board;
}

inline void Game::Draw_Game() const noexcept
{
    static constexpr LCD_I2C::byte FIRST_COLUMN = 0;
    static constexpr LCD_I2C::byte SECOND_COLUMN = 2;
    static constexpr LCD_I2C::byte THIRD_COLUMN = 4;
    static constexpr LCD_I2C::byte FOURTH_COLUMN = 6;

    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        lcd->SetCursor(row, FIRST_COLUMN);
        lcd->PrintCustomChar(LOCATION_LEFT);
        lcd->SetCursor(row, SECOND_COLUMN);
        lcd->PrintCustomChar(LOCATION_CENTER);
        lcd->SetCursor(row, THIRD_COLUMN);
        lcd->PrintCustomChar(LOCATION_CENTER);
        lcd->SetCursor(row, FOURTH_COLUMN);
        lcd->PrintCustomChar(LOCATION_RIGHT);
    }
}

inline void Game::Draw_Board_State() const noexcept
{
    static constexpr LCD_I2C::byte FIRST_COLUMN = 1;
    static constexpr LCD_I2C::byte SECOND_COLUMN = 3;
    static constexpr LCD_I2C::byte THIRD_COLUMN = 5;

    #pragma GCC unroll 3
    for (size_t row = 0; row < BOARD_SIZE; ++row)
    {
        lcd->SetCursor(row, FIRST_COLUMN);
        lcd->PrintCustomChar(LCD_Char_Location_From_Board_State(game_board[row][0]));
        lcd->SetCursor(row, SECOND_COLUMN);
        lcd->PrintCustomChar(LCD_Char_Location_From_Board_State(game_board[row][1]));
        lcd->SetCursor(row, THIRD_COLUMN);
        lcd->PrintCustomChar(LCD_Char_Location_From_Board_State(game_board[row][2]));
    }
}

inline void Game::Print_Winner_And_Update_Score(Player winner) noexcept
{
    static constexpr size_t AFTER_WIN_DELAY = 5000;

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("GAME OVER  ");

    if (winner == PLAYER_UNKNOWN)
    {
        lcd->SetCursor(1, TEXT_START_COLUMN);
        lcd->PrintString("   TIE     ");
    }
    else
    {
        lcd->SetCursor(1, TEXT_START_COLUMN);
        lcd->PrintString("  ");
        if (winner == PLAYER_X)
        {
            Increase_X_Score();
            lcd->PrintCustomChar(LOCATION_X);
        }
        else
        {
            Increase_0_Score();
            lcd->PrintCustomChar(LOCATION_0);
        }
        lcd->PrintString(" WINS   ");
    }

    sleep_ms(AFTER_WIN_DELAY);
}

inline void Game::Print_User_Info(Player current_player) const noexcept
{
    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString(" Your turn");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString(" Play as ");
    if (current_player == PLAYER_X)
    {
        lcd->PrintCustomChar(LOCATION_X);
    }
    else if (current_player == PLAYER_0)
    {
        lcd->PrintCustomChar(LOCATION_0);
    }
    lcd->PrintCustomChar(LOCATION_SPACE);
}

inline void Game::Print_Computer_Info() const noexcept
{
    static constexpr size_t DOTS_START_COLUMN = 16;
    static constexpr size_t DELAY = 200;

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("  Computer");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString("thinking   ");
    lcd->SetCursor(1, DOTS_START_COLUMN);
    sleep_ms(DELAY);
    lcd->PrintString(".");
    sleep_ms(DELAY);
    lcd->PrintString(".");
    sleep_ms(DELAY);
    lcd->PrintString(".");
    sleep_ms(DELAY);
}

void Game::Internal_Play() noexcept
{
    Player user = PLAYER_UNKNOWN;

    while (true)
    {
        if (user == PLAYER_UNKNOWN)
        {
            user = Get_User();
        }
        else
        {
            static bool game_over;
            static Player current_player;

            Draw_Board_State();

            game_over = Is_Terminal(game_board);
            current_player = Get_Current_Player(game_board);

            if (game_over)
            {
                Print_Winner_And_Update_Score(Get_Winner(game_board));
                Reset_Board();
                Draw_Board_State();
                break;
            }
            if (user == current_player)
            {
                Print_User_Info(current_player);
            }
            else
            {
                Print_Computer_Info();
            }

            if (user != current_player)
            {
                if (ai_turn)
                {
                    auto move = game_strategy->GetNextMove(game_board);
                    game_board = Get_Result_Board(game_board, move, Get_Current_Player(game_board));
                    ai_turn = false;
                }
                else
                {
                    ai_turn = true;
                }
            }
            else if (user == current_player)
            {
                static Action move;

                do
                {
                    move = Action_From_Key(keypad->GetPressedKey());
                }
                while (!Is_Valid_Action(game_board, move));
                game_board = Get_Result_Board(game_board, move, Get_Current_Player(game_board));
            }
        }
    }
}

inline void Game::Choose_Difficulty() noexcept
{
    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("  Choose  ");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString("difficulty ");

    do
    {
        game_strategy.reset(Difficulty_From_Key(keypad->GetPressedKey()));
    }
    while (game_strategy == nullptr);
}

inline auto Game::Get_User() const noexcept -> Game::Player
{
    static Player choice;

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("  Choose");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString("  ");
    lcd->PrintCustomChar(LOCATION_X);
    lcd->PrintString(" or ");
    lcd->PrintCustomChar(LOCATION_0);
    lcd->PrintString("   ");

    do
    {
        choice = Player_From_Key(keypad->GetPressedKey());
    }
    while (choice == PLAYER_UNKNOWN);

    return choice;
}

void Game::Backlight_And_Reset_Runner() noexcept
{
    static bool light_on {false};
    static Key key {};

    auto * keypad = reinterpret_cast<Keypad *> (multicore_fifo_pop_blocking());
    auto * lcd = reinterpret_cast<LCD_I2C *>(multicore_fifo_pop_blocking());
    auto * game = reinterpret_cast<Game *>(multicore_fifo_pop_blocking());

    while (true)
    {
        key = keypad->GetPressedKey();
        if (key == Key::KEY14)
        {
            light_on = !light_on;
            lcd->SetBacklight(light_on);
        }
        else if (key == Key::KEY13)
        {
            game->Reset_Scoreboard();
        }
    }
}

inline void Game::Update_Scoreboard() const noexcept
{
    led_segments->DisplayLeft(score_X, true);
    led_segments->DisplayRight(score_0, true);
}

inline void Game::Increase_X_Score() noexcept
{
    ++score_X;
    Update_Scoreboard();
}

inline void Game::Increase_0_Score() noexcept
{
    ++score_0;
    Update_Scoreboard();
}

inline void Game::Reset_Scoreboard() noexcept
{
    score_X = 0;
    score_0 = 0;
    Update_Scoreboard();
}

auto Game::Action_From_Key(Key key) noexcept -> Action
{
    switch (key)
    {
        case Key::KEY1:
            return {0, 0};
        case Key::KEY2:
            return {0, 1};
        case Key::KEY3:
            return {0, 2};
        case Key::KEY5:
            return {1, 0};
        case Key::KEY6:
            return {1, 1};
        case Key::KEY7:
            return {1, 2};
        case Key::KEY9:
            return {2, 0};
        case Key::KEY10:
            return {2, 1};
        case Key::KEY11:
            return {2, 2};
        default:
            return {-1, -1};
    }
}

auto Game::Player_From_Key(Key key) noexcept -> Game::Player
{
    switch (key)
    {
        case Key::KEY15:
            return PLAYER_X;
        case Key::KEY16:
            return PLAYER_0;
        default:
            return PLAYER_UNKNOWN;
    }
}

auto Game::Difficulty_From_Key(Key key) noexcept -> Game::IGameStrategy *
{
    switch (key)
    {
        case Key::KEY4:
            return new EasyStrategy;
        case Key::KEY8:
            return new MediumStrategy;
        case Key::KEY12:
            return new ImpossibleStrategy;
        default:
            return nullptr;
    }
}

[[noreturn]] void Game::Play() noexcept
{
    Draw_Game();

    led_segments->ColonOn();
    Update_Scoreboard();

    while (true)
    {
        Choose_Difficulty();
        Internal_Play();
    }
}

Game::IGameStrategy::IGameStrategy() noexcept : random_number_generator(Get_Random_Seed()) {}

inline auto Game::IGameStrategy::Get_Random_Seed() noexcept -> uint32_t
{
    static constexpr uint32_t FNV_OFFSET_BASIS = 0x811C9DC5;
    static constexpr uint32_t FNV_PRIME = 0x01000193;
    static constexpr size_t NO_OF_ROUNDS = 16;
    static constexpr size_t NO_OF_BYTES = 8;

    uint32_t random = FNV_OFFSET_BASIS;
    uint8_t next_byte = 0;
    auto * volatile rnd_reg = reinterpret_cast<uint32_t *>(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

    for (size_t i = 0; i < NO_OF_ROUNDS; i++)
    {
        for (size_t k = 0; k < NO_OF_BYTES; k++)
        {
            next_byte = (next_byte << 1) | (*rnd_reg & 1);
        }
        random ^= next_byte;
        random *= FNV_PRIME;
    }
    return random;
}

auto Game::IGameStrategy::GetRNG() noexcept -> std::mt19937 &
{
    return random_number_generator;
}

auto Game::EasyStrategy::GetNextMove(Board const & current_board) noexcept -> Action
{
    if (Is_Terminal(current_board))
    {
        return {};
    }

    auto actions = Get_Actions(current_board);

    std::sample(actions.begin(), actions.end(), std::back_inserter(actions), 1, GetRNG());
    return actions.back();
}

auto Game::MediumStrategy::GetNextMove(Board const & current_board) noexcept -> Action
{
    if (Is_Terminal(current_board))
    {
        return {};
    }

    auto actions = Get_Actions(current_board);

    for (auto const & action: actions)
    {
        if (Is_Winner(PLAYER_X, Get_Result_Board(current_board, action, PLAYER_X)) ||
                Is_Winner(PLAYER_0, Get_Result_Board(current_board, action, PLAYER_0)))
        {
            return action;
        }
    }

    std::sample(actions.begin(), actions.end(), std::back_inserter(actions), 1, GetRNG());
    return actions.back();
}

auto Game::ImpossibleStrategy::Get_Min_Value(Board const & current_board, Value alpha, Value beta) const noexcept
-> Game::Value
{
    if (Is_Terminal(current_board))
    {
        return Get_Board_Value(current_board);
    }

    Value value = VALUE_MAX;

    auto current_actions = Get_Actions(current_board);
    for (Action const & action: current_actions)
    {
        value = std::min(value, Get_Max_Value(Get_Result_Board(current_board, action,
                                                               Get_Current_Player(current_board)), alpha, beta));
        beta = std::min(beta, value);
        if (value <= alpha)
        {
            return value;
        }
    }
    return value;
}

auto Game::ImpossibleStrategy::Get_Max_Value(Board const & current_board, Value alpha, Value beta) const noexcept
-> Game::Value
{
    if (Is_Terminal(current_board))
    {
        return Get_Board_Value(current_board);
    }

    Value value = VALUE_MIN;

    auto current_actions = Get_Actions(current_board);
    for (Action const & action: current_actions)
    {
        value = std::max(value, Get_Min_Value(Get_Result_Board(current_board, action,
                                                               Get_Current_Player(current_board)), alpha, beta));
        alpha = std::max(alpha, value);
        if (value >= beta)
        {
            return value;
        }
    }
    return value;
}

auto Game::ImpossibleStrategy::Get_Possible_Moves(Game::Board const & current_board) const
-> std::unordered_map<Action, Game::Value, Action::Hash>
{
    auto actions = Get_Actions(current_board);

    std::unordered_map<Action, Value, Action::Hash> possible_moves;
    if (Get_Current_Player(current_board) == PLAYER_X)
    {
        Value max_value = VALUE_MIN;
        for (Action const & action: actions)
        {
            possible_moves.emplace(action, Get_Min_Value(Get_Result_Board(current_board, action,
                                                                          Get_Current_Player(current_board)),
                                                         VALUE_MIN, VALUE_MAX));
            if (possible_moves[action] > max_value)
            {
                max_value = possible_moves[action];
            }
        }
        for (auto it = possible_moves.begin(); it != possible_moves.end();)
        {
            if (it->second != max_value)
            {
                it = possible_moves.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    else
    {
        Value min_value = VALUE_MAX;
        for (Action const & action: actions)
        {
            possible_moves.emplace(action, Get_Max_Value(Get_Result_Board(current_board, action,
                                                                          Get_Current_Player(current_board)),
                                                         VALUE_MIN, VALUE_MAX));
            if (possible_moves[action] < min_value)
            {
                min_value = possible_moves[action];
            }
        }
        for (auto it = possible_moves.begin(); it != possible_moves.end();)
        {
            if (it->second != min_value)
            {
                it = possible_moves.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    return possible_moves;
}

auto Game::ImpossibleStrategy::GetNextMove(Board const & current_board) noexcept -> Action
{
    if (Is_Terminal(current_board))
    {
        return {};
    }

    auto possible_moves = Get_Possible_Moves(current_board);

    std::vector<std::pair<Action, Value>> result {possible_moves.begin(), possible_moves.end()};
    for (auto const &[ACTION, VALUE]: result)
    {
        if (Is_Winner(Get_Current_Player(current_board), Get_Result_Board(current_board, ACTION,
                                                                          Get_Current_Player(current_board))))
        {
            return ACTION;
        }
    }
    std::sample(result.begin(), result.end(), std::back_inserter(result), 1, GetRNG());
    return result.back().first;
}
