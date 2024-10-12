#pragma once
#include <SFML/Graphics.hpp>
#include "button.h"
#include "constants.h"

class MainMenu
{
	sf::RenderWindow &win;
	sf::Font font;
	Settings settings;

	textButton colorBtn;
	textButton multiplayerBtn;
	textButton computerBtn;
	textButton playBtn;
	imgButton whiteBtn;
	imgButton blackBtn;
	imgButton randomBtn;

	void setupButtons();
public:
	bool isComplete = false;

	MainMenu(sf::RenderWindow& win);
	void run();
	void draw();
	Settings getSettings();
};