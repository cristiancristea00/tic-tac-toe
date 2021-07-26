/*******************************************************************************
 * @file Game.cpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Source file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Game.hpp"

Game::Game(LCD_I2C * lcd, TM1637 * led_segments) noexcept : lcd(lcd), led_segments(led_segments)
{
    static constexpr int NO_CUSTOM_SYMBOLS = 7;

    static constexpr LCD_I2C::byte CUSTOM_SYMBOLS[NO_CUSTOM_SYMBOLS][8] =
            {{0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07}, /* LEFT */
             {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F}, /* CENTER */
             {0x1C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1C}, /* RIGHT */
             {0x00, 0x11, 0x0A, 0x04, 0x04, 0x0A, 0x11, 0x00}, /* X */
             {0x00, 0x0E, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00}, /* 0 */
             {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* ' ' */
             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}  /* ■ */};

    for (int location = 0; location < NO_CUSTOM_SYMBOLS; ++location)
    {
        lcd->CreateCustomChar(location, CUSTOM_SYMBOLS[location]);
    }
}

inline uint32_t Game::Get_Random_Seed() noexcept
{
    uint32_t random = 0x811c9dc5;
    uint8_t next_byte = 0;
    volatile uint32_t * rnd_reg = reinterpret_cast<unsigned long *>(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

    for (int i = 0; i < 16; i++)
    {
        for (int k = 0; k < 8; k++)
        {
            next_byte = (next_byte << 1) | (*rnd_reg & 1);
        }
        random ^= next_byte;
        random *= 0x01000193;
    }
    return random;
}

inline Game::BoardState Game::Board_State_From_Player(Player player) noexcept
{
    if (player == PLAYER_X)
    {
        return BoardState::X;
    }
    else if (player == PLAYER_0)
    {
        return BoardState::O;
    }
    else
    {
        return BoardState::UNKNOWN;
    }
}

inline char Game::Char_From_Board_State(BoardState board_state) noexcept
{
    switch (board_state)
    {
        case BoardState::X:
            return 'X';
        case BoardState::O:
            return '0';
        default:
            return ' ';
    }
}

LCD_I2C::byte Game::LCD_Char_Location_From_Board_State(BoardState board_state) noexcept
{
    switch (board_state)
    {
        case BoardState::X:
            return 3;
        case BoardState::O:
            return 4;
        default:
            return 5;
    }
}

void Game::Reset_Board() noexcept
{
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int column = 0; column < BOARD_SIZE; ++column)
        {
            game_board[row][column] = BoardState::EMPTY;
        }
    }
}

inline bool Game::Is_Board_Full(Board const & current_board) noexcept
{
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] == BoardState::EMPTY)
            {
                return false;
            }
        }
    }
    return true;
}

bool Game::Is_Winner(Player current_player, Board const & current_board) noexcept
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

Game::Player Game::Get_Current_Player(Board const & current_board) noexcept
{
    uint8_t moves = 0;
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] != BoardState::EMPTY)
            {
                ++moves;
            }
        }
    }
    return (moves % 2 == 0) ? PLAYER_X : PLAYER_0;
}

inline std::vector<Action> Game::Get_Actions(Board const & current_board) noexcept
{
    static std::vector<Action> actions;
    actions.reserve(BOARD_SIZE * BOARD_SIZE);

    actions.clear();
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int column = 0; column < BOARD_SIZE; ++column)
        {
            if (current_board[row][column] == BoardState::EMPTY)
            {
                actions.emplace_back(row, column);
            }
        }
    }
    return actions;
}

inline Game::Player Game::Get_Winner(Board const & current_board) noexcept
{
    if (Is_Winner(PLAYER_X, current_board))
    {
        return PLAYER_X;
    }
    else if (Is_Winner(PLAYER_0, current_board))
    {
        return PLAYER_0;
    }
    else
    {
        return PLAYER_UNKNOWN;
    }
}

inline bool Game::Is_Terminal(Board const & current_board) noexcept
{
    return Is_Board_Full(current_board) || Is_Winner(PLAYER_X, current_board) || Is_Winner(PLAYER_0, current_board);
}

inline bool Game::Is_Valid_Action(Board const & current_board, Action const & action) noexcept
{
    return action.row >= 0 && action.column >= 0 && action.row < BOARD_SIZE && action.column < BOARD_SIZE
            && current_board[action.row][action.column] == BoardState::EMPTY;
}

inline Game::Value Game::Get_Board_Value(Board const & current_board) noexcept
{
    if (Is_Winner(PLAYER_X, current_board))
    {
        return 1;
    }
    else if (Is_Winner(PLAYER_0, current_board))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

inline Game::Board Game::Get_Result_Board(Board const & current_board, Action const & action) noexcept
{
    auto action_board = current_board;
    action_board[action.row][action.column] = Board_State_From_Player(Get_Current_Player(current_board));
    return action_board;
}

inline Action Game::Get_Best_Move(Board const & current_board) const noexcept
{
    static auto random_number_generator = std::mt19937 {Get_Random_Seed()};

    if (Is_Terminal(current_board))
    {
        return Action();
    }

    auto actions = Get_Actions(current_board);
    std::unordered_map<Action, Value, Action::Hash> possible_moves;
    if (Get_Current_Player(current_board) == PLAYER_X)
    {
        Value max_value = VALUE_MIN;
        for (Action const & action : actions)
        {
            possible_moves.emplace(action, Get_Min_Value(Get_Result_Board(current_board, action),
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
        for (Action const & action : actions)
        {
            possible_moves.emplace(action, Get_Max_Value(Get_Result_Board(current_board, action),
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
    std::vector<std::pair<Action, Value>> result(possible_moves.begin(), possible_moves.end());
    for (auto const &[ACTION, VALUE] : result)
    {
        if (Is_Winner(Get_Current_Player(current_board), Get_Result_Board(current_board, ACTION)))
        {
            return ACTION;
        }
    }
    std::sample(result.begin(), result.end(), std::back_inserter(result), 1, random_number_generator);
    return result.back().first;
}

Game::Value Game::Get_Min_Value(Board const & current_board, Value alpha, Value beta) const noexcept
{
    if (Is_Terminal(current_board))
    {
        return Get_Board_Value(current_board);
    }

    Value value = VALUE_MAX;

    auto current_actions = Get_Actions(current_board);
    for (Action const & action : current_actions)
    {
        value = std::min(value, Get_Max_Value(Get_Result_Board(current_board, action), alpha, beta));
        beta = std::min(beta, value);
        if (value <= alpha)
        {
            return value;
        }
    }
    return value;
}

Game::Value Game::Get_Max_Value(Board const & current_board, Value alpha, Value beta) const noexcept
{
    if (Is_Terminal(current_board))
    {
        return Get_Board_Value(current_board);
    }

    Value value = VALUE_MIN;

    auto current_actions = Get_Actions(current_board);
    for (Action const & action : current_actions)
    {
        value = std::max(value, Get_Min_Value(Get_Result_Board(current_board, action), alpha, beta));
        alpha = std::max(alpha, value);
        if (value >= beta)
        {
            return value;
        }
    }
    return value;
}

inline void Game::Draw_Game() const noexcept
{
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        lcd->SetCursor(row, 0);
        lcd->PrintCustomChar(0);
        lcd->SetCursor(row, 2);
        lcd->PrintCustomChar(1);
        lcd->SetCursor(row, 4);
        lcd->PrintCustomChar(1);
        lcd->SetCursor(row, 6);
        lcd->PrintCustomChar(2);
    }
}

void Game::Draw_Board_State() const noexcept
{
    static constexpr std::string_view HORIZONTAL_SEPARATOR = "-------------\n";
    static constexpr std::string_view GRID_FORMAT = "| %c | %c | %c |\n";

    printf("%s", HORIZONTAL_SEPARATOR.data());
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        printf(GRID_FORMAT.data(), Char_From_Board_State(game_board[row][0]),
               Char_From_Board_State(game_board[row][1]),
               Char_From_Board_State(game_board[row][2]));
        printf("%s", HORIZONTAL_SEPARATOR.data());

        lcd->SetCursor(row, 1);
        lcd->PrintCustomChar(LCD_Char_Location_From_Board_State(game_board[row][0]));
        lcd->SetCursor(row, 3);
        lcd->PrintCustomChar(LCD_Char_Location_From_Board_State(game_board[row][1]));
        lcd->SetCursor(row, 5);
        lcd->PrintCustomChar(LCD_Char_Location_From_Board_State(game_board[row][2]));
    }
}

void Game::Internal_Play() noexcept
{
    while (true)
    {
        if (user == PLAYER_UNKNOWN)
        {
            static Player choice;

            lcd->SetCursor(0, 10);
            lcd->PrintString("Choose ");
            lcd->SetCursor(1, 10);
            lcd->PrintCustomChar(3);
            lcd->PrintString(" or ");
            lcd->PrintCustomChar(4);
            lcd->PrintChar(' ');

            printf("Choose X or 0:\n");

            scanf(" %c", &choice);
            if (toupper(choice) == 'X')
            {
                user = PLAYER_X;
            }
            else if (toupper(choice) == '0')
            {
                user = PLAYER_0;
            }
            else
            {
                lcd->SetCursor(0, 10);
                lcd->PrintString("Invalid");
                lcd->SetCursor(1, 10);
                lcd->PrintString("player!");

                printf("Invalid player!\n");
                sleep_ms(3000);
                break;
            }
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
                Player winner = Get_Winner(game_board);
                if (winner == PLAYER_UNKNOWN)
                {
                    printf("Game Over: TIE\n");
                }
                else
                {
                    printf("Game Over: %c wins\n", winner);
                    if (winner == PLAYER_X)
                    {
                        Increase_X_Score();
                    }
                    else
                    {
                        Increase_0_Score();
                    }
                }
                break;
            }
            else if (user == current_player)
            {
                printf("Play as %c\n", user);
            }
            else
            {
                printf("Computer thinking...\n");
            }

            if (user != current_player)
            {
                if (ai_turn)
                {
                    Action move = Get_Best_Move(game_board);
                    game_board = Get_Result_Board(game_board, move);
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
                    printf("Choose coordinates:\n");
                    scanf(" %hd %hd", &move.row, &move.column);
                    if (!Is_Valid_Action(game_board, move))
                    {
                        printf("Invalid coordinates!\n");
                    }
                }
                while (!Is_Valid_Action(game_board, move));
                game_board = Get_Result_Board(game_board, move);
            }
        }
    }
}

void Game::Update_Scoreboard() const noexcept
{
    led_segments->DisplayLeft(score_X, true);
    led_segments->DisplayRight(score_0, true);
}

void Game::Increase_X_Score() noexcept
{
    ++score_X;
    Update_Scoreboard();
}

void Game::Increase_0_Score() noexcept
{
    ++score_0;
    Update_Scoreboard();
}

[[noreturn]] void Game::Play() noexcept
{
    lcd->BacklightOn();
    Draw_Game();

    led_segments->ColonOn();
    Update_Scoreboard();

    while (true)
    {
        Reset_Board();
        Internal_Play();
    }
}
