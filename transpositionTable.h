#pragma once
#include "board.h"

// Hash Flag Constants
#define HASH_EXACT 0
#define HASH_ALPHA 1
#define HASH_BETA  2
#define SCORE_UNKNOWN -32767

// Structure for the Transposition Table
struct HashEntry {
    U64 key;        // Zobrist key to verify the position
    int score;      // Evaluation score
    int depth;      // Depth this position was searched to
    int flags;      // Type of node (Exact, Alpha, or Beta)
    Move bestMove;  // Best move found at this position (Critical for Move Ordering)
};

// Table size: 2^20 = ~1 million entries (adjust based on available RAM)
const int TABLE_SIZE = 1048576;

// Global declarations (defined in search.cpp)
extern HashEntry transTable[TABLE_SIZE];
extern U64 pieceArr[12][64];
extern U64 castleArr[5];
extern U64 epArr[8];
extern U64 blackMove;