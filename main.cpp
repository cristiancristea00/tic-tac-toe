#include <pico/binary_info/code.h>
#include <pico/stdlib.h>
#include "LCD_I2C.hpp"
#include "Game.hpp"

int main()
{
    constexpr auto SDA = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr auto SCL = PICO_DEFAULT_I2C_SCL_PIN;
    constexpr auto I2C = PICO_DEFAULT_I2C_INSTANCE;

    bi_decl(bi_2pins_with_func(SDA, SCL, GPIO_FUNC_I2C));

    stdio_uart_init();

    auto lcd = new LCD_I2C(0x27, 20, 4, SDA, SCL, I2C);
    auto game = new Game(lcd);

    game->Play();
}
