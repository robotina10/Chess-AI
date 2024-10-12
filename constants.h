#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

const int WIN_WIDTH = 800, WIN_HEIGHT = 800;
const int ROWS = 8, COLS = 8;
const int SIDE = WIN_WIDTH / ROWS;

const sf::Color WHITE(255, 255, 255);
const sf::Color BLACK(128, 128, 128);
const sf::Color HIGHLIGHTED_WHITE(175, 155, 175);
const sf::Color HIGHLIGHTED_BLACK(88, 88, 98);

enum Mode { AI, MULTIPLAYER };

struct Settings
{
	Mode mode = AI;
	bool whiteTurn = true;
};

typedef std::uint64_t U64;

enum Pieces { bKing, wKing, bQueen, wQueen, bRook, wRook, bBishop, wBishop, bKnight, wKnight, bPawn, wPawn, Blacks, Whites, EMPTY };
enum Color { Black, White };


namespace pieceColor {
	inline int getPieceColor(Pieces piece)
	{
		switch (piece) {
		case bKing:
		case bQueen:
		case bRook:
		case bBishop:
		case bKnight:
		case bPawn:
			return Black;
		case wKing:
		case wQueen:
		case wRook:
		case wBishop:
		case wKnight:
		case wPawn:
			return White;
		}
	}

	inline int getOpponentGroup(Pieces piece) 
	{
		switch (getPieceColor(piece)) {
		case Black:
			return Whites;
		case White:
			return Blacks;
		}
	}
}