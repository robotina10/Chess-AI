#pragma once
#include <iostream>
#include "game.h"
#include "mainMenu.h"
#include "constants.h"
//currently using optimatisation for space because for speed it gives bad perft result (because of order of instructions) {step by debbuger through whole program, mainly board (king after capture problem)}
//on configure tepmlete and on notebook for all configurations https://www.sfml-dev.org/tutorials/2.6/start-vc.php also realese mode
//rename .h after class names 
//gpu performace, drawing???
//look at stack and heap allocation for speed
int main()
{
    sf::RenderWindow win(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Chess", sf::Style::Close);
    win.setFramerateLimit(60);

    Settings settings;
    MainMenu mainMenu(win);
    mainMenu.run();

    Game game(win, mainMenu.getSettings());
    game.startGame();

    return 0;
}
