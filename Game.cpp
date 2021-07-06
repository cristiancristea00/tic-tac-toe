/*******************************************************************************
 * @file Game.cpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Source file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Game.hpp"

namespace std
{
template <>
struct hash<Action>
{
    std::size_t operator()(Action const & action) const noexcept
    {
        return std::hash<uint8_t>()(action.row) ^ std::hash<uint8_t>()(action.column);
    }
};
}

Game::Game() noexcept
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            game_board[i][j] = BoardState::EMPTY;
        }
    }
}

Game::BoardState Game::BoardStateFromPlayer(Game::Player player) noexcept
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

char Game::CharFromBoardState(BoardState board_state) noexcept
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

bool Game::Is_Board_Full(Board const & current_board) noexcept
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (current_board[i][j] == BoardState::EMPTY)
            {
                return false;
            }
        }
    }
    return true;
}

bool Game::Is_Winner(Player current_player, Board const & current_board) noexcept
{
    BoardState symbol = BoardStateFromPlayer(current_player);
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
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (current_board[i][j] != BoardState::EMPTY)
            {
                ++moves;
            }
        }
    }
    return (moves % 2 == 0) ? PLAYER_X : PLAYER_0;
}

std::vector<Action> Game::Get_Actions(Board const & current_board) noexcept
{
    std::vector<Action> actions;
    actions.reserve(BOARD_SIZE * BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (current_board[i][j] == BoardState::EMPTY)
            {
                actions.emplace_back(i, j, Action::Type::VALID);
            }
        }
    }
    return actions;
}

Game::Board Game::Get_Result_Board(Board const & current_board, Action const & action)
{
    if (current_board[action.row][action.column] != BoardState::EMPTY || action.type == Action::Type::INVALID)
    {
        throw std::invalid_argument("Invalid action!");
    }
    auto action_board = current_board;
    action_board[action.row][action.column] = BoardStateFromPlayer(Get_Current_Player(current_board));
    return action_board;
}

Game::Player Game::Get_Winner(Board const & current_board) noexcept
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

bool Game::Is_Terminal(Board const & current_board) noexcept
{
    return Is_Board_Full(current_board) || Is_Winner(PLAYER_X, current_board) || Is_Winner(PLAYER_0, current_board);
}

Game::Value Game::Utility(Board const & current_board) noexcept
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

Game::Value Game::Get_Min_Value(Board const & current_board) const noexcept
{
    if (Is_Terminal(current_board))
    {
        return Utility(current_board);
    }
    Value value = std::numeric_limits<Value>::max();
    auto current_actions = Get_Actions(current_board);
    for (Action const & action : current_actions)
    {
        value = std::min(value, Get_Max_Value(Get_Result_Board(current_board, action)));
    }
    return value;
}

Game::Value Game::Get_Max_Value(Board const & current_board) const noexcept
{
    if (Is_Terminal(current_board))
    {
        return Utility(current_board);
    }
    Value value = std::numeric_limits<Value>::min();
    auto current_actions = Get_Actions(current_board);
    for (Action const & action : current_actions)
    {
        value = std::max(value, Get_Min_Value(Get_Result_Board(current_board, action)));
    }
    return value;
}

Action Game::Minimax(Board const & current_board) const noexcept
{
    if (Is_Terminal(current_board))
    {
        return Action(Action::Type::INVALID);
    }

    auto actions = Get_Actions(current_board);
    std::unordered_map<Action, Value> possible_moves;
    if (Get_Current_Player(current_board) == PLAYER_X)
    {
        Value max_value = std::numeric_limits<Value>::min();
        for (Action const & action : actions)
        {
            possible_moves.emplace(action, Get_Min_Value(Get_Result_Board(current_board, action)));
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
        Value min_value = std::numeric_limits<Value>::max();
        for (Action const & action : actions)
        {
            possible_moves.emplace(action, Get_Max_Value(Get_Result_Board(current_board, action)));
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
    std::sample(result.begin(), result.end(), std::back_inserter(result), 1, std::mt19937_64 {std::random_device {}()});
    return result.back().first;
}

void Game::DrawBoard() const noexcept
{
    using fmt::print;
    static constexpr std::string_view horizontal_separator = "-------------\n";
    static constexpr std::string_view grid_format = "| {} | {} | {} |\n";

    print(horizontal_separator);
    print(grid_format, CharFromBoardState(game_board[0][0]),
          CharFromBoardState(game_board[0][1]),
          CharFromBoardState(game_board[0][2]));
    print(horizontal_separator);
    print(grid_format, CharFromBoardState(game_board[1][0]),
          CharFromBoardState(game_board[1][1]),
          CharFromBoardState(game_board[1][2]));
    print(horizontal_separator);
    print(grid_format, CharFromBoardState(game_board[2][0]),
          CharFromBoardState(game_board[2][1]),
          CharFromBoardState(game_board[2][2]));
    print(horizontal_separator);
}

void Game::Play()
{
    using fmt::print;

    while (true)
    {
        if (user == PLAYER_UNKNOWN)
        {
            Player choice;
            print("Choose between X or 0: ");
            scanf("%c", &choice);
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
                throw std::invalid_argument("Invalid player");
            }
        }
        else
        {
            static bool game_over;
            static Player current_player;

            DrawBoard();

            game_over = Is_Terminal(game_board);
            current_player = Get_Current_Player(game_board);

            if (game_over)
            {
                Player winner = Get_Winner(game_board);
                if (winner == PLAYER_UNKNOWN)
                {
                    print("Game Over: TIE\n");
                }
                else
                {
                    print("Game Over: {:c} wins\n", winner);
                }
                break;
            }
            else if (user == current_player)
            {
                print("Play as {:c}\n", user);
            }
            else
            {
                print("Computer thinking...\n");
            }

            if (user != current_player)
            {
                if (ai_turn)
                {
                    Action move = Minimax(game_board);
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
                Action move(Action::Type::VALID);
                print("Choose coordinates: ");
                scanf("%u %u", &move.row, &move.column);
                game_board = Get_Result_Board(game_board, move);
            }
        }
        system("clear");
    }
}
