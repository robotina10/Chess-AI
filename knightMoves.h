#pragma once
#include "board.h"

U64 knightAttacks[64];

const U64 notABFile = 0x0;
const U64 notGHFile = 0x0;
const U64 notAFFile = 0x0;

U64 noNoEa(U64 b) { return (b << 17) & notAFile; }
U64 noEaEa(U64 b) { return (b << 10) & notABFile; }
U64 soEaEa(U64 b) { return (b >> 6) & notABFile; }
U64 soSoEa(U64 b) { return (b >> 15) & notAFile; }
U64 noNoWe(U64 b) { return (b << 15) & notHFile; }
U64 noWeWe(U64 b) { return (b << 6) & notGHFile; }
U64 soWeWe(U64 b) { return (b >> 10) & notGHFile; } 
U64 soSoWe(U64 b) { return (b >> 17) & notHFile; }

void initKnightAttacks()
{
	for (int i = 0; i < 64; i++) {
		U64 bb = (1ULL << i);
		U64 attack = noNoEa(bb) | noEaEa(bb) | soEaEa(bb) | soSoEa(bb) | noNoWe(bb) | noWeWe(bb) | soWeWe(bb) | soSoWe(bb);
		knightAttacks[i] = attack;
	}
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
				moveList.moves[moveList.count++] = Move(from, to, knight, EMPTY, NONE);
			}
			else if ((1ULL << to) & enemy) {
				capturedPiece = getPiece(to);
				moveList.moves[moveList.count++] = Move(from, to, knight, capturedPiece, NONE);
			}
		}
	}
}
