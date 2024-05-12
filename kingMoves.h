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
					if (!(occupied & 0x0600000000000000))
						moveList.moves[moveList.count++] = Move(60, 62, king, EMPTY, KING_CASTLING);
			}
			if (getCastlingRight(wQueenSide)) {
				if (!(attacked(60, whiteTurn) || attacked(59, whiteTurn) || attacked(58, whiteTurn) || attacked(57, whiteTurn)))
					if (!(occupied & 0x0e00000000000000))
						moveList.moves[moveList.count++] = Move(60, 57, king, EMPTY, QUEEN_CASTLING);
			}
		}
		else {
			if (getCastlingRight(bKingSide)) {
				if (!(attacked(4, whiteTurn) || attacked(5, whiteTurn) || attacked(6, whiteTurn)))
					if (!(occupied & 0x60))
						moveList.moves[moveList.count++] = Move(4, 6, king, EMPTY, KING_CASTLING);
			}
			if (getCastlingRight(bQueenSide)) {
				if (!(attacked(4, whiteTurn) || attacked(3, whiteTurn) || attacked(2, whiteTurn) || attacked(1, whiteTurn)))
					if (!(occupied & 0xe))
						moveList.moves[moveList.count++] = Move(4, 1, king, EMPTY, QUEEN_CASTLING);
			}
		}
	}
}

void Board::getKingMoves(Pieces king, MoveList& moveList)
{
	if (bb[king]) {
		int from = bitScanForward(bb[king]);
		U64 attack = kingAttacks[from] & (getEmpty() | getEnemy(king));
		//U64 enemyAttack = getEnemyAttack(whiteTurn);
		//attack &= enemyAttack;
		while (attack) {
			int to = bitScanForwardWithReset(attack);
			if (inCheck(to, whiteTurn))
				continue;
			if ((1ULL << to) & occupied) {
				moveList.moves[moveList.count++] = Move(from, to, king, getPiece(to), NONE);
			}
			else {
				moveList.moves[moveList.count++] = Move(from, to, king, EMPTY, NONE);
			}
		}
	}
	getCastlingMoves(king, moveList);
}