#pragma once
#include "board.h"

U64 kingAttacks[64];

void initKingAttacks()
{
	for (int i = 0; i < 64; i++) {
		U64 king = 1ULL << i;
		U64 attack = eastOne(king) | westOne(king);
		king |= attack;
		kingAttacks[i] = attack | northOne(king) | southOne(king);
	}
}

void Board::getCastlingMoves(Pieces king, MoveList& moveList)
{
	if (castlingRights) {
		if (king == wKing) {
			if (getCastlingRight(wKingSide)) {
				if (!(attacked(60, whiteTurn) || attacked(61, whiteTurn) || attacked(62, whiteTurn)))
					if (!(occupied & 0x6000000000000000))
						moveList.moves[moveList.count++] = Move(60, 62, king, EMPTY, KING_CASTLING);
			}
			if (getCastlingRight(wQueenSide)) {
				if (!(attacked(60, whiteTurn) || attacked(59, whiteTurn) || attacked(58, whiteTurn)))
					if (!(occupied & 0xe00000000000000))
						moveList.moves[moveList.count++] = Move(60, 58, king, EMPTY, QUEEN_CASTLING);
			}
		}
		else {
			if (getCastlingRight(bKingSide)) {
				if (!(attacked(4, whiteTurn) || attacked(5, whiteTurn) || attacked(6, whiteTurn)))
					if (!(occupied & 0x60))
						moveList.moves[moveList.count++] = Move(4, 6, king, EMPTY, KING_CASTLING);
			}
			if (getCastlingRight(bQueenSide)) {
				if (!(attacked(4, whiteTurn) || attacked(3, whiteTurn) || attacked(2, whiteTurn)))
					if (!(occupied & 0xe))
						moveList.moves[moveList.count++] = Move(4, 2, king, EMPTY, QUEEN_CASTLING);
			}
		}
	}
}

void Board::getKingMoves(Pieces king, MoveList& moveList)
{
	int from = bitScanForward(bb[king]);
	U64 attack = kingAttacks[from] & (getEmpty() | getEnemy(king));
	attack ^= attack & checkedSquares(whiteTurn);
	while (attack) {
		int to = bitScanForwardWithReset(attack);
		if ((1ULL << to) & occupied) {
			moveList.moves[moveList.count++] = Move(from, to, king, getPiece(to), EMPTY);
		}
		else {
			moveList.moves[moveList.count++] = Move(from, to, king, EMPTY, EMPTY);
		}
	}
	getCastlingMoves(king, moveList);
}