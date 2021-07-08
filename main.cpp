#include <pico/binary_info/code.h>
#include <pico/stdlib.h>
#include "LCD_I2C.hpp"
#include "Game.hpp"

int main()
{
    constexpr uint SDA_pin = PICO_DEFAULT_I2C_SDA_PIN;
    constexpr uint SCL_pin = PICO_DEFAULT_I2C_SCL_PIN;

    bi_decl(bi_2pins_with_func(SDA_pin, SCL_pin, GPIO_FUNC_I2C));
    stdio_uart_init();

    auto lcd = new LCD_I2C(0x27, 20, 4, SDA_pin, SCL_pin, i2c_default);
    // lcd->BacklightOn();
    lcd->SetCursor(0, 0);
    lcd->Print("Hello");
    lcd->SetCursor(1, 0);
    lcd->Print("Claudia");
    lcd->SetCursor(2, 0);
    lcd->Print("Cristi are mere.");
    while (true);
}
