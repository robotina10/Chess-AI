#pragma once
#include <stdlib.h>  
#include <time.h> 
#include <optional>
#include "mainMenu.h"

const sf::Color textCol(58, 58, 58);

MainMenu::MainMenu(sf::RenderWindow& win) : win(win),
	colorBtn("Choose color", font, { 0, 200 }, 25, textCol),
	whiteBtn("img/wk.png", { 0, 260 }, 1, 1),
	blackBtn("img/bk.png", { 0, 260 }, 1, 1),
	randomBtn("img/random.png", { 0, 260 }, 1, 1),
	multiplayerBtn("MULTIPLAYER", font, { 0, 350 }, 25, textCol),
	computerBtn("COMPUTER", font, { 0, 420 }, 25, textCol),
	playBtn("PLAY", font, { 0, 490 }, 25, textCol)
{
	// SFML 3: Font loading now uses openFromFile instead of loadFromFile
	if (!font.openFromFile("font/celtig.ttf")) {
		std::cerr << "Cannot load font: celtig.ttf \n";
		exit(1);
	}
	setupButtons();
}

void MainMenu::setupButtons()
{
	const sf::Color fillCol(245, 245, 245);
	const sf::Color outlineCol(200, 200, 200);
	float outlineThickness = 5;

	colorBtn.setRect({ -10, -10 }, { 20, 20 }, fillCol, outlineThickness, outlineCol, true);
	float x = colorBtn.getPos().x;
	whiteBtn.setRect({x, 0}, {0, 0}, fillCol, outlineThickness, outlineCol);
	x = whiteBtn.getPos().x + whiteBtn.getSize().x;
	blackBtn.setRect({ x + 10, 0 }, { 0, 0 }, fillCol, outlineThickness, outlineCol);
	x = blackBtn.getPos().x + whiteBtn.getSize().x;
	randomBtn.setRect({ x + 10, 0 }, { 0, 0 }, fillCol, outlineThickness, outlineCol);
	multiplayerBtn.setRect({ -10, -10 }, { 20, 20 }, fillCol, outlineThickness, outlineCol, true);
	computerBtn.setRect({ -10, -10 }, { 20, 20 }, fillCol, outlineThickness, outlineCol, true);
	playBtn.setRect({ -10, -10 }, { 20, 20 }, fillCol, outlineThickness, outlineCol, true);
}

void MainMenu::run()
{
	srand(time(0));

	while (win.isOpen()) 
	{
		while (auto event = win.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				win.close();
			}
			else if (const auto mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
			{
				sf::Vector2i pos = mouseButtonPressed->position;
				if (whiteBtn.isClicked(pos)) {
					settings.whiteView = true;
				}
				else if (blackBtn.isClicked(pos)) {
					settings.whiteView = false;
				}
				else if (randomBtn.isClicked(pos)) {
					if (rand() % 10 < 5)
						settings.whiteView = true;
					else
						settings.whiteView = false;
				}
				else if (multiplayerBtn.isClicked(pos)) {
					settings.mode = MULTIPLAYER;
				}
				else if (computerBtn.isClicked(pos)) {
					settings.mode = AI;
				}
				else if (playBtn.isClicked(pos)) {
					isComplete = true;
					return;
				}
			}
		}
		draw();
	}
}

void MainMenu::draw()
{
	win.clear(sf::Color(120, 120, 120));
	win.draw(colorBtn);
	win.draw(whiteBtn);
	win.draw(blackBtn);
	win.draw(randomBtn);
	win.draw(multiplayerBtn);
	win.draw(computerBtn);
	win.draw(playBtn);
	win.display();
}

Settings MainMenu::getSettings()
{
	return settings;
}
