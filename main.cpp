#include "pico/stdlib.h"
#include "Game.hpp"

int main()
{
    stdio_uart_init();
    while (true)
    {
        auto game = new Game();
        game->Play();
        delete game;
    }
    return 0;
}
