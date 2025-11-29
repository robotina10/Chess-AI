#pragma once
#include <math.h>
#include <random>
#include "board.h"

#define scoreUNKNOWN -1
#define hashEXACT 0
#define hashALPHA 1
#define hashBETA  2

typedef struct Hash
{
	U64 key;
	int depth;
	int flags;
	int score;
	Move bestMove;
};

const int tableSize = 1048576; //2**20  2**17 131072
Hash transTable[tableSize] = {};

void Board::recordHash(int val, int hashFlag, int depth)
{
	Hash* hash = &transTable[ZobristKey() & tableSize];
	hash->key = ZobristKey();
	hash->bestMove = Move();// bestMove();
	hash->score = val;
	hash->flags = hashFlag;
	hash->depth = depth;
}

int Board::retrieveHash(int alpha, int beta, int depth)
{
	Hash* hash = &transTable[ZobristKey() & tableSize];
	if (hash->key == ZobristKey()) {
		if (hash->depth == depth) {
			if (hash->flags == hashEXACT)
				return hash->score;
			if (hash->flags == hashALPHA && hash->score <= alpha)
				return alpha;
			if (hash->flags == hashBETA && hash->score >= beta)
				return beta;
		}
		//rememberBestMove();
	}
	return scoreUNKNOWN;
}

U64 pieceArr[12][64];
U64 castleArr[5];
U64 epArr[8];
U64 blackMove;

void Board::fillZobristArrs()
{
	std::random_device rd;
	std::mt19937_64 generator(rd());
	std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 64; j++) {
			pieceArr[i][j] = distribution(generator);
		}
	}
	castleArr[0] = 0;
	for (int i = 1; i < 5; i++) {
		castleArr[i] = distribution(generator);
	}
	for (int i = 0; i < 8; i++) {
		epArr[i] = distribution(generator);
	}
	blackMove = distribution(generator);
}

U64 Board::ZobristKey()
{
	U64 key = 0;
	for (int i = 0; i < 12; i++) {
		int pos;
		U64 pieces = bb[i];
		while (pos = bitScanForwardWithReset(pieces))
			key ^= pieceArr[i][pos];
	}
	if (!whiteTurn)
		key ^= blackMove;
	key ^= castleArr[getCastlingRight(wKingSide)];
	key ^= castleArr[getCastlingRight(wQueenSide)];
	key ^= castleArr[getCastlingRight(bKingSide)];
	key ^= castleArr[getCastlingRight(bQueenSide)];
	key ^= epArr[enPassantSquare % 8];
	return key;
}