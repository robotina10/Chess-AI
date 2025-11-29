#pragma once
#include "board.h"

U64 knightAttacks[64];

const U64 notABFile = ~0x0303030303030303;
const U64 notGHFile = ~0xc0c0c0c0c0c0c0c0;
const U64 notAFFile = ~0x8181818181818181;

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
		U64 knight = (1ULL << i);
		knightAttacks[i] = noNoEa(knight) | noEaEa(knight) | soEaEa(knight) | soSoEa(knight) | noNoWe(knight) | noWeWe(knight) | soWeWe(knight) | soSoWe(knight);
	}
}

void Board::getKnightMoves(Piece knight, MoveList &moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly)
{
	U64 knights = bb[knight];  
	knights = (knights ^ pinnedPieces.all) & knights;
	while (knights) {
		int from = bitScanForwardWithReset(knights);
		U64 attack = knightAttacks[from] & (getEmpty() | getEnemy(knight));
		attack &= checkingPieces;
		while (attack) {
			int to = bitScanForwardWithReset(attack);
			Move move;
			if ((1ULL << to) & occupied)
				moveList.moves[moveList.count++] = { from, to, knight, getPiece(to), EMPTY };
			else if (!capturesOnly)
				moveList.moves[moveList.count++] = { from, to, knight, EMPTY, EMPTY };
		}
	}
}
