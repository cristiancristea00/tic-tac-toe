#include <pico/binary_info/code.h>
#include <pico/stdlib.h>

#include <memory>

#include "Game.hpp"
#include "Keypad.hpp"

auto main() -> int
{
    constexpr auto I2C = PICO_DEFAULT_I2C_INSTANCE;
    constexpr auto SDA = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr auto SCL = PICO_DEFAULT_I2C_SCL_PIN;
    constexpr auto DIO = 27;
    constexpr auto CLK = 28;
    constexpr Keypad::array ROWS {10, 11, 12, 13};
    constexpr Keypad::array COLUMNS {18, 19, 20, 21};
    auto pio = pio0;

    bi_decl(bi_1pin_with_name(SDA, "[SDA] LCD screen data pin"))
    bi_decl(bi_1pin_with_name(SCL, "[SCL] LCD screen clock pin"))
    bi_decl(bi_1pin_with_name(DIO, "[DIO] Scoreboard data pin"))
    bi_decl(bi_1pin_with_name(CLK, "[CLK] Scoreboard clock pin"))
    bi_decl(bi_1pin_with_name(ROWS[0], "[R1] Keypad first row pin"))
    bi_decl(bi_1pin_with_name(ROWS[1], "[R2] Keypad second row pin"))
    bi_decl(bi_1pin_with_name(ROWS[2], "[R3] Keypad third row pin"))
    bi_decl(bi_1pin_with_name(ROWS[3], "[R4] Keypad fourth row pin"))
    bi_decl(bi_1pin_with_name(COLUMNS[0], "[C1] Keypad first column pin"))
    bi_decl(bi_1pin_with_name(COLUMNS[1], "[C2] Keypad second column pin"))
    bi_decl(bi_1pin_with_name(COLUMNS[2], "[C3] Keypad third column pin"))
    bi_decl(bi_1pin_with_name(COLUMNS[3], "[C4] Keypad fourth column pin"))

    stdio_init_all();

    auto game = std::make_unique<Game>(
            new LCD_I2C {0x27, 20, 4, I2C, SDA, SCL},
            new TM1637 {DIO, CLK, pio},
            new Keypad {ROWS, COLUMNS});
    game->Play();
}

