#pragma once
#include "board.h"

#define SCORE_UNKNOWN -32767
#define HASH_EXACT 0
#define HASH_ALPHA 1
#define HASH_BETA  2

struct HashEntry
{
	U64 key;
	int score;
	int depth;
	int flags;
	Move bestMove;
};

const int TABLE_SIZE = 1048576; //2 ^ 20 entries

extern HashEntry transTable[TABLE_SIZE];
extern U64 pieceArr[12][64];
extern U64 castleArr[16];
extern U64 epArr[8];
extern U64 blackMove;