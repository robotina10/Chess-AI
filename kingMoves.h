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

void Board::getKingMoves(Pieces king, MoveList& moveList)
{
	if (bb[king]) {
		int from = bitScanForward(bb[king]);
		U64 attack = kingAttacks[from] & (getEmpty() | getEnemy(king));
		while (attack) {
			int to = bitScanForwardWithReset(attack);
			if ((1ULL << to) & occupied) {
				moveList.moves[moveList.count++] = Move(from, to, king, getPiece(to), NONE);
			}
			else {
				moveList.moves[moveList.count++] = Move(from, to, king, EMPTY, NONE);
			}
		}
	}
}