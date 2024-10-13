#pragma once
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "draw.h"
#include "chessEngine.h"
#include "button.h"

enum class Sounds { CAPTURE_SOUND, MOVE_SOUND };

struct MoveInfo
{
	bool isPressed = false;
	bool mouseMoved = false;
	int from = -1;
	int to = -1;
	int moveIndex = -1;
	SpecialMove promotion = NONE;
};

class Game
{
	sf::RenderWindow& win;
	ChessEngine chess;
	Settings settings;
	MoveInfo moveInfo;

	void init();
	int calcSquarePos(sf::Vector2i pos);
	void playSound(Sounds soundType);
	int getMoveIndex();
	void makeMove();
	void computer();
	//void draw();


public:
	Game(sf::RenderWindow& win, Settings settings) : win(win), settings(settings) {}
	void startGame();
};