#include <iostream>
#include "Game.hpp"

int main()
{
    auto game = new Game();
    game->Play();
    delete game;
    return 0;
}
