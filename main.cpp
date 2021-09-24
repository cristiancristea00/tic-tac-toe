#include <pico/binary_info/code.h>

#include "Keypad.hpp"
#include "Game.hpp"

int main()
{
    constexpr auto I2C = PICO_DEFAULT_I2C_INSTANCE;
    constexpr auto SDA = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr auto SCL = PICO_DEFAULT_I2C_SCL_PIN;
    constexpr auto I2C_ADDRESS = 0x27;
    constexpr auto LCD_COLUMNS = 20;
    constexpr auto LCD_ROWS = 4;
    constexpr auto DIO = 27;
    constexpr auto CLK = 28;
    constexpr Keypad::array KEYPAD_ROWS {10, 11, 12, 13};
    constexpr Keypad::array KEYPAD_COLUMNS {18, 19, 20, 21};
    auto * pio = pio0;

    bi_decl(bi_1pin_with_name(SDA, "[SDA] LCD screen data pin"))
    bi_decl(bi_1pin_with_name(SCL, "[SCL] LCD screen clock pin"))
    bi_decl(bi_1pin_with_name(DIO, "[DIO] Scoreboard data pin"))
    bi_decl(bi_1pin_with_name(CLK, "[CLK] Scoreboard clock pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_ROWS[0], "[R1] Keypad first row pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_ROWS[1], "[R2] Keypad second row pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_ROWS[2], "[R3] Keypad third row pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_ROWS[3], "[R4] Keypad fourth row pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_COLUMNS[0], "[C1] Keypad first column pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_COLUMNS[1], "[C2] Keypad second column pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_COLUMNS[2], "[C3] Keypad third column pin"))
    bi_decl(bi_1pin_with_name(KEYPAD_COLUMNS[3], "[C4] Keypad fourth column pin"))

    auto game = std::make_unique<Game>(
            new LCD_I2C {I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS, I2C, SDA, SCL},
            new TM1637 {DIO, CLK, pio},
            new Keypad {KEYPAD_ROWS, KEYPAD_COLUMNS});

    game->Play();
}

