#pragma once
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <optional>
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

	int draggedPieceIndex = -1;
	sf::Vector2f dragPos;
};

class Game
{
	sf::RenderWindow& win;
	ChessEngine chess;
	Settings settings;
	MoveState moveState;

	std::optional<sf::Sound> soundPlayer;

	SpecialMove selectPromotion(int toSquare);

	sf::Font font;
	bool handleGameOver();

	void init();
	int calcSquarePos(sf::Vector2i pos);
	void playSound();
	int getMoveIndex();
	void makeMove();
	void unMakeMove();
	void computer();
	int getPieceIndexAt(int square);
	void draw();


public:
	Game(sf::RenderWindow& win, Settings settings) : win(win), settings(settings) {}
	void startGame();
};