#pragma once
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "draw.h"
#include "chessEngine.h"
#include "button.h"
#include "draw.h"
//#include "perft.h"

enum Sounds {CAPTURE_SOUND, MOVE_SOUND};

struct MoveInfo
{
	bool isPressed = false;
	bool mouseMoved = false;
	int from = -1;
	Pieces selectedPiece = EMPTY;
};

class Game
{
	sf::RenderWindow& win;
	ChessEngine chess;
	Settings settings;

	void init();
	int calcSquarePos(sf::Vector2i pos);
	void playSound(Sounds soundType);
	int getMoveIndex(int from, int to);
	void makeMove(int& from, int to, Pieces& piece, int i, SpecialMove promotion);
	void computer();
	//void draw();


public:
	Game(sf::RenderWindow& win) : win(win) {}
	void startGame();
	void setSettings(Settings settings);
};