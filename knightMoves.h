#pragma once
#include "board.h"

U64 knightAttacks[64];
//https://www.chessprogramming.org/Knight_Pattern
void initAttacksArr()
{

}

void Board::getKnightMoves(Pieces knight, MoveList &moveList)
{
	U64 knights = bb[knight];
	U64 enemy = bb[(knight == wKnight) ? Blacks : Whites];

	while (knights) {
		int from = bitScanForwardWithReset(knights);
		U64 attack = knightAttacks[from];
		while (attack) {
			int to = bitScanForwardWithReset(attack);
			Pieces capturedPiece = EMPTY;

			if ((1ULL << to) & getEmpty()) {
				moveList.moves[moveList.count++] = Move(from, to, knight, EMPTY, 0, 0);
			}
			else if ((1ULL << to) & enemy) {
				capturedPiece = getPiece(to);
				moveList.moves[moveList.count++] = Move(from, to, knight, capturedPiece, 0, 0);
			}
		}
	}
}
