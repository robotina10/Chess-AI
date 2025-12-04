#pragma once
#include <iostream>
#include "game.h"
#include "mainMenu.h"
#include "constants.h"

int main()
{
    sf::RenderWindow win(sf::VideoMode({WIN_WIDTH, WIN_HEIGHT}), "Chess", sf::Style::Close);
    win.setFramerateLimit(60);

    Settings settings;
    MainMenu mainMenu(win);

    while (win.isOpen()) {
        while (auto event = win.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                win.close();
            }
            if (!mainMenu.isComplete) {
                mainMenu.run();
                mainMenu.draw();
            }
            if (mainMenu.isComplete) {
                Game game(win, mainMenu.getSettings());
                game.startGame();
                mainMenu.isComplete = false;
            }
        }
    }
    return 0;
}
