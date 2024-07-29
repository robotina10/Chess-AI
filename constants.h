#pragma once
#include <iostream>

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