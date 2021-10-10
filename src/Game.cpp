/*******************************************************************************
 * @file Game.cpp
 * @author Cristian Cristea
 * @date July 04, 2021
 * @brief Source file for the Game class.
 *
 * @copyright Copyright (C) 2021 Cristian Cristea. All rights reserved.
 ******************************************************************************/

#include "Game.hpp"

#include <memory>

using Utility::PlayerSymbol;
using Utility::BOARD_SIZE;

Game::Game(LCD_I2C * lcd, TM1637 * led_segments, Keypad * keypad) noexcept
        : lcd(lcd), led_segments(led_segments), keypad(keypad)
{
    static constexpr size_t NO_SYMBOLS = 6;

    static constexpr std::array<std::array<byte, LCD_I2C::CUSTOM_SYMBOL_SIZE>, NO_SYMBOLS> CUSTOM_SYMBOLS
            {{{0x07, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x07}, /* LEFT */
              {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F}, /* CENTER */
              {0x1C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1C}, /* RIGHT */
              {0x00, 0x11, 0x0A, 0x04, 0x04, 0x0A, 0x11, 0x00}, /* X */
              {0x00, 0x0E, 0x11, 0x11, 0x11, 0x11, 0x0E, 0x00}, /* 0 */
              {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  /* ' ' */}};

    # pragma GCC unroll 6
    for (byte location = 0; location < NO_SYMBOLS; ++location)
    {
        lcd->CreateCustomChar(location, CUSTOM_SYMBOLS.at(location));
    }

    Init_Second_Core();
}

void Game::Init_Second_Core() const noexcept
{
    multicore_launch_core1(Key_Poller_Runner);
    multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(keypad.get()));
    multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(lcd.get()));
    multicore_fifo_push_blocking(reinterpret_cast<uint32_t>(led_segments.get()));
}

auto Game::LCD_Char_Location_From_Player_Symbol(PlayerSymbol symbol) noexcept -> byte
{
    switch (symbol)
    {
        case PlayerSymbol::X:
            return LOCATION_X;
        case PlayerSymbol::O:
            return LOCATION_0;
        default:
            return LOCATION_SPACE;
    }
}

inline void Game::Draw_Game() const noexcept
{
    static constexpr byte FIRST_COLUMN = 0;
    static constexpr byte SECOND_COLUMN = 2;
    static constexpr byte THIRD_COLUMN = 4;
    static constexpr byte FOURTH_COLUMN = 6;

    #pragma GCC unroll 3
    for (byte row = 0; row < BOARD_SIZE; ++row)
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
    static constexpr byte FIRST_COLUMN = 1;
    static constexpr byte SECOND_COLUMN = 3;
    static constexpr byte THIRD_COLUMN = 5;

    #pragma GCC unroll 3
    for (byte row = 0; row < BOARD_SIZE; ++row)
    {
        lcd->SetCursor(row, FIRST_COLUMN);
        lcd->PrintCustomChar(LCD_Char_Location_From_Player_Symbol(
                BoardManager::Instance()->GetGameBoard()[row][0]));
        lcd->SetCursor(row, SECOND_COLUMN);
        lcd->PrintCustomChar(LCD_Char_Location_From_Player_Symbol(
                BoardManager::Instance()->GetGameBoard()[row][1]));
        lcd->SetCursor(row, THIRD_COLUMN);
        lcd->PrintCustomChar(LCD_Char_Location_From_Player_Symbol(
                BoardManager::Instance()->GetGameBoard()[row][2]));
    }
}

inline void Game::Print_Winner_And_Update_Score(PlayerSymbol winner) noexcept
{
    static constexpr size_t AFTER_WIN_DELAY = 5000;

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString(" GAME OVER  ");
    lcd->SetCursor(1, TEXT_START_COLUMN);

    if (winner == PlayerSymbol::UNK)
    {
        lcd->PrintString("   TIE     ");
    }
    else
    {
        if (winner == first_player->GetSymbol())
        {
            Increase_First_Player_Score();
            lcd->PrintString("  You won ");
        }
        else
        {
            Increase_Second_Player_Score();
            lcd->PrintString(" The other ");
            lcd->SetCursor(2, TEXT_START_COLUMN);
            lcd->PrintString(" player won");
        }
    }

    sleep_ms(AFTER_WIN_DELAY);
}

inline void Game::Print_First_Player_Info() const noexcept
{
    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString(" Your turn ");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString(" Play as ");
    if (first_player->GetSymbol() == PlayerSymbol::X)
    {
        lcd->PrintCustomChar(LOCATION_X);
    }
    else
    {
        lcd->PrintCustomChar(LOCATION_0);
    }
    lcd->PrintCustomChar(LOCATION_SPACE);
    lcd->SetCursor(2, TEXT_START_COLUMN);
    lcd->PrintString("          ");
}

inline void Game::Print_Second_Player_Info() const noexcept
{
    static constexpr size_t DOTS_START_COLUMN = 16;
    static constexpr size_t DELAY = 200;

    if (second_player->GetStrategyName() != "HUMAN")
    {
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
    else
    {
        lcd->SetCursor(0, TEXT_START_COLUMN);
        lcd->PrintString("The other's");
        lcd->SetCursor(1, TEXT_START_COLUMN);
        lcd->PrintString("player turn");
        lcd->SetCursor(2, TEXT_START_COLUMN);
        lcd->PrintString(" Play as ");
        if (second_player->GetSymbol() == PlayerSymbol::X)
        {
            lcd->PrintCustomChar(LOCATION_X);
        }
        else
        {
            lcd->PrintCustomChar(LOCATION_0);
        }
    }
}

void Game::Internal_Play() noexcept
{
    static bool second_player_turn {false};

    first_player->SetSymbol(PlayerSymbol::UNK);

    while (true)
    {
        if (first_player->GetSymbol() == PlayerSymbol::UNK)
        {
            static PlayerSymbol first_player_symbol;

            first_player_symbol = Get_User();
            first_player->SetSymbol(first_player_symbol);
            if (first_player_symbol == PlayerSymbol::X)
            {
                second_player->SetSymbol(PlayerSymbol::O);
            }
            else
            {
                second_player->SetSymbol(PlayerSymbol::X);
            };
        }
        else
        {
            static bool game_over {false};
            static PlayerSymbol current_player;

            Draw_Board_State();

            game_over = BoardManager::Instance()->IsTerminal(BoardManager::Instance()->GetGameBoard());
            current_player = BoardManager::Instance()->GetCurrentPlayer(BoardManager::Instance()->GetGameBoard());

            if (game_over)
            {
                Print_Winner_And_Update_Score(BoardManager::Instance()->GetWinner(BoardManager::Instance()->GetGameBoard()));
                BoardManager::Instance()->ResetBoard();
                Draw_Board_State();
                Continue_After_Game();
                break;
            }
            if (first_player->GetSymbol() == current_player)
            {
                Print_First_Player_Info();
            }
            else
            {
                Print_Second_Player_Info();
            }

            if (first_player->GetSymbol() != current_player)
            {
                if (second_player_turn)
                {
                    auto move = second_player->GetNextMove(BoardManager::Instance()->GetGameBoard());
                    BoardManager::Instance()->GetGameBoard() = BoardManager::Instance()->GetResultBoard(
                            BoardManager::Instance()->GetGameBoard(), move, second_player->GetSymbol());
                    second_player_turn = false;
                }
                else
                {
                    second_player_turn = true;
                }
            }
            else if (first_player->GetSymbol() == current_player)
            {
                static Move move;

                move = first_player->GetNextMove(BoardManager::Instance()->GetGameBoard());
                BoardManager::Instance()->GetGameBoard() = BoardManager::Instance()->GetResultBoard(
                        BoardManager::Instance()->GetGameBoard(), move, first_player->GetSymbol());
            }
        }
    }
}

inline auto Game::Get_Difficulty() noexcept -> IPlayerStrategy *
{
    static IPlayerStrategy * strategy;

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("  Choose   ");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString("difficulty ");
    lcd->SetCursor(3, TEXT_START_COLUMN);
    lcd->PrintString("           ");

    do
    {
        strategy = Keypad::DifficultyFromKey(keypad->GetPressedKey());
    }
    while (strategy == nullptr);

    return strategy;
}

inline auto Game::Get_User() const noexcept -> PlayerSymbol
{
    static PlayerSymbol choice;

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("  Choose    ");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString("  ");
    lcd->PrintCustomChar(LOCATION_X);
    lcd->PrintString(" or ");
    lcd->PrintCustomChar(LOCATION_0);
    lcd->PrintString("   ");
    lcd->SetCursor(2, TEXT_START_COLUMN);
    lcd->PrintString("           ");

    do
    {
        choice = Keypad::PlayerFromKey(keypad->GetPressedKey());
    }
    while (choice == PlayerSymbol::UNK);

    return choice;
}

void Game::Key_Poller_Runner() noexcept
{
    bool light_on {false};
    Key key {Key::UNKNOWN};
    uint8_t brightness {0};

    auto * keypad = reinterpret_cast<Keypad *> (multicore_fifo_pop_blocking());
    auto * lcd = reinterpret_cast<LCD_I2C *>(multicore_fifo_pop_blocking());
    auto * led_segments = reinterpret_cast<TM1637 *>(multicore_fifo_pop_blocking());

    while (true)
    {
        key = keypad->GetKeyFromPoller();
        if (key == Key::KEY13)
        {
            light_on = !light_on;
            lcd->SetBacklight(light_on);
        }
        else if (key == Key::KEY14)
        {
            brightness = (++brightness) % TM1637::MAX_BRIGHTNESS;
            led_segments->SetBrightness(brightness);
        }
        else
        {
            multicore_fifo_push_blocking(static_cast<uint32_t>(key));
        }
    }
}

inline void Game::Update_Scoreboard() const noexcept
{
    led_segments->DisplayLeft(score.first, true);
    led_segments->DisplayRight(score.second, true);
}

inline void Game::Increase_First_Player_Score() noexcept
{
    ++score.first;
    Update_Scoreboard();
}

inline void Game::Increase_Second_Player_Score() noexcept
{
    ++score.second;
    Update_Scoreboard();
}

inline void Game::Reset_Scoreboard() noexcept
{
    score = {0, 0};
    Update_Scoreboard();
}

[[noreturn]] void Game::Play() noexcept
{
    Draw_Game();

    led_segments->ColonOn();
    Update_Scoreboard();

    Choose_Enemy();

    while (true)
    {
        Internal_Play();
    }
}

inline void Game::Print_Difficulty(std::string_view diff) noexcept
{
    lcd->SetCursor(3, TEXT_START_COLUMN);
    lcd->PrintString("Diff:");
    lcd->PrintString(diff);
}

void Game::Choose_Enemy() noexcept
{
    static std::string_view choice {};

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("Play versus ");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString("HUMAN or AI");
    lcd->SetCursor(2, TEXT_START_COLUMN);
    lcd->PrintString("            ");

    do
    {
        choice = Keypad::EnemyFromKey(keypad->GetPressedKey());
    }
    while (choice.empty());

    if (choice == "HUMAN")
    {
        second_player = std::make_unique<Player>(PlayerSymbol::UNK, new HumanStrategy);
    }
    else
    {
        second_player = std::make_unique<Player>(PlayerSymbol::UNK, Get_Difficulty());
        Print_Difficulty(second_player->GetStrategyName());
    }
}

void Game::Continue_After_Game() noexcept
{
    static std::string_view answer {};

    lcd->SetCursor(0, TEXT_START_COLUMN);
    lcd->PrintString("Keep playing");
    lcd->SetCursor(1, TEXT_START_COLUMN);
    lcd->PrintString(" with the ");
    lcd->SetCursor(2, TEXT_START_COLUMN);
    lcd->PrintString("same enemy?");
    lcd->SetCursor(3, TEXT_START_COLUMN);
    lcd->PrintString("            ");

    do
    {
        answer = Keypad::AnswerFromKey(keypad->GetPressedKey());
    }
    while (answer.empty());

    if (answer == "NO")
    {
        Reset_Scoreboard();
        Choose_Enemy();
    }
    else
    {
        if (second_player->GetStrategyName() != "HUMAN")
        {
            Print_Difficulty(second_player->GetStrategyName());
        }
        return;
    }
}
