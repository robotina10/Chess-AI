#pragma once
#include <iostream>

typedef std::uint64_t U64;

enum Pieces { bKing, wKing, bQueen, wQueen, bRook, wRook, bBishop, wBishop, bKnight, wKnight, bPawn, wPawn, Blacks, Whites, EMPTY };
//enum Color { Black, White };


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
			return Blacks;
		case wKing:
		case wQueen:
		case wRook:
		case wBishop:
		case wKnight:
		case wPawn:
			return Whites;
		}
	}

	inline int getPieceColor(int piece)
	{
		switch (piece) {
		case bKing:
		case bQueen:
		case bRook:
		case bBishop:
		case bKnight:
		case bPawn:
			return Blacks;
		case wKing:
		case wQueen:
		case wRook:
		case wBishop:
		case wKnight:
		case wPawn:
			return Blacks;
		}
	}

	inline int getOpponentColor(Pieces piece) 
	{
		switch (getPieceColor(piece)) {
		case Blacks:
			return Whites;
		case Whites:
			return Blacks;
		}
	}
}