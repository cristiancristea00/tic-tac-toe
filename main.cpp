#include <pico/binary_info/code.h>
#include <pico/stdlib.h>
#include "Game.hpp"

int main()
{
    constexpr auto I2C = PICO_DEFAULT_I2C_INSTANCE;
    constexpr auto SDA = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr auto SCL = PICO_DEFAULT_I2C_SCL_PIN;
    constexpr auto DIO = 27;
    constexpr auto CLK = 28;
    auto pio = pio0;

    bi_decl(bi_1pin_with_name(SDA, "[SDA] LCD screen data pin"));
    bi_decl(bi_1pin_with_name(SCL, "[SCL] LCD screen clock pin"));
    bi_decl(bi_1pin_with_name(DIO, "[DIO] Scoreboard data pin"));
    bi_decl(bi_1pin_with_name(CLK, "[CLK] Scoreboard clock pin"));

    auto display = new LCD_I2C(0x27, 20, 4, I2C, SDA, SCL);
    auto scoreboard = new TM1637(DIO, CLK, pio);

    stdio_init_all();

    auto game = new Game(display, scoreboard);
    game->Play();
}
