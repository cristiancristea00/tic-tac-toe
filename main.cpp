#include <iostream>
#include "Game.hpp"

int main()
{
    while (true)
    {
        auto game = new Game();
        game->Play();
        delete game;
    }
    return 0;
}
