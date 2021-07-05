#include <iostream>
#include "Game.hpp"

int main()
{
    auto game = new Game();
    game->DrawBoard();
    delete game;
    return 0;
}
