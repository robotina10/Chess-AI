#pragma once
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "chessEngine.h"
#include "button.h"

enum class Sounds { CAPTURE_SOUND, MOVE_SOUND };

struct MoveState
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
	MoveState moveState;

	imgButton promSelectionBtn;
	SpecialMove promPieceSelected(sf::Vector2f btnPos, int squareClicked);

	void init();
	int calcSquarePos(sf::Vector2i pos);
	void playSound(Sounds soundType);
	int getMoveIndex();
	void makeMove();
	void unMakeMove();
	void computer();
	void draw();


public:
	Game(sf::RenderWindow& win, Settings settings) : win(win), settings(settings), promSelectionBtn("img/prom.png", { 0, 0 }, 1.17, 1.183) {}
	void startGame();
};