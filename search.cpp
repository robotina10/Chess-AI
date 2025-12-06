#include <chrono>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <random>
#include "board.h"
#include "transpositionTable.h"


HashEntry transTable[TABLE_SIZE];
U64 pieceArr[12][64];
U64 castleArr[16];
U64 epArr[8];
U64 blackMove;

const int MATE_SCORE = -1000000;
const int DRAW_SCORE = 0;
const int MAX_PLY = 128;
const int INF = 1000000000;
const int MAX_DEPTH = 64;

Move killerMoves[MAX_PLY][2];
int historyHeuristic[2][64][64];

// MVV-LVA: Most Valuable Victim - Least Valuable Attacker
// (Victim: P, N, B, R, Q, K) x (Attacker: P, N, B, R, Q, K)
int mvv_lva[6][6] = {
    {0, 0, 0, 0, 0, 0},         // Victim K (invalid)
    {50, 51, 52, 53, 54, 55},   // Victim Q
    {40, 41, 42, 43, 44, 45},   // Victim R
    {30, 31, 32, 33, 34, 35},   // Victim B
    {20, 21, 22, 23, 24, 25},   // Victim N
    {10, 11, 12, 13, 14, 15},   // Victim P
};

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
    for (int i = 1; i < 16; i++) {
        castleArr[i] = distribution(generator);
    }

    for (int i = 0; i < 8; i++) {
        epArr[i] = distribution(generator);
    }

    blackMove = distribution(generator);

    std::memset(transTable, 0, sizeof(HashEntry) * TABLE_SIZE);
}

U64 Board::ZobristKey()
{
    U64 key = 0;

    for (int i = 0; i < 12; i++) {
        U64 pieces = bb[i];
        while (pieces) {
            int pos = bitScanForwardWithReset(pieces);
            key ^= pieceArr[i][pos];
        }
    }

    if (!whiteTurn) {
        key ^= blackMove;
    }

    key ^= castleArr[castlingRights];

    if (enPassantSquare) {
        key ^= epArr[enPassantSquare % 8];
    }

    return key;
}


void pickMove(MoveList& moveList, int startIndex) {
    int bestIndex = startIndex;
    for (int i = startIndex + 1; i < moveList.count; i++) {
        if (moveList.scores[i] > moveList.scores[bestIndex]) {
            bestIndex = i;
        }
    }
    std::swap(moveList.moves[startIndex], moveList.moves[bestIndex]);
    std::swap(moveList.scores[startIndex], moveList.scores[bestIndex]);
}

void Board::scoreMoves(MoveList& moveList, int ply, Move ttMove)
{
    for (int i = 0; i < moveList.count; i++) {
        Move move = moveList.moves[i];
        int score = 0;

        if (move == ttMove) {
            score = 2000000;
        }
        else if (move.isCapture()) {
            int victim = move.getCapturedPiece() >> 1;
            int attacker = move.getPiece() >> 1;
            score = 1000000 + mvv_lva[victim][attacker];
        }
        else if (move.isPromotion() && move.getSpecialMove() == QUEEN_PROM) {
            score = 950000;
        }
        else {
            if (killerMoves[ply][0] == move)
                score = 900000;
            else if (killerMoves[ply][1] == move)
                score = 800000;
            else
                score = historyHeuristic[move.getPieceColor()][move.getTo()][move.getFrom()];
        }
        moveList.scores[i] = score;
    }
}


int Board::QuiescenceSearch(int alpha, int beta)
{
    int standPat = eval();
    if (standPat >= beta) return beta;

    int BIG_DELTA = 1200; // queen value

    if (standPat < alpha - BIG_DELTA) {
        return alpha;
    }

    if (alpha < standPat) alpha = standPat;

    MoveList moveList(218);
    generateLegalMoves(moveList, true); 

    scoreMoves(moveList, 0);

    PosInfo posInfo(castlingRights, enPassantSquare, halfMoveClock);

    for (int i = 0; i < moveList.count; i++) {
        pickMove(moveList, i);
        Move move = moveList.moves[i];

        makeMove(move);
        int score = -QuiescenceSearch(-beta, -alpha);
        unMakeMove(move, posInfo);

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int Board::alphaBeta(int alpha, int beta, int depthLeft, int ply)
{
    if (ply >= MAX_PLY) return eval();

    if (ply > 0 && (threeFoldRepetitionRule() || fiftyMoveRule())) {
        return DRAW_SCORE;
    }

    U64 key = currentHash;
    HashEntry* ttEntry = &transTable[key & (TABLE_SIZE - 1)];
    bool ttHit = (ttEntry->key == key);
    Move ttMove;

    if (ttHit) {
        ttMove = ttEntry->bestMove;
        if (ttEntry->depth >= depthLeft && ply > 0) {
            if (ttEntry->flags == HASH_EXACT) return ttEntry->score;
            if (ttEntry->flags == HASH_ALPHA && ttEntry->score <= alpha) return alpha;
            if (ttEntry->flags == HASH_BETA && ttEntry->score >= beta) return beta;
        }
    }

    if (depthLeft <= 0) return QuiescenceSearch(alpha, beta);

    MoveList moveList(218);
    generateLegalMoves(moveList);

    if (moveList.count == 0) {
        if (isCheck()) return MATE_SCORE + ply;
        return DRAW_SCORE;
    }

    scoreMoves(moveList, ply, (ttHit) ? ttMove: Move());
    if (ttHit && !ttMove.isNone()) {
        for (int i = 0; i < moveList.count; i++) {
            if (moveList.moves[i] == ttMove) {
                moveList.scores[i] = 2000000;
                break;
            }
        }
    }

    PosInfo posInfo(castlingRights, enPassantSquare, halfMoveClock);
    int bestScore = -INF;
    int startAlpha = alpha;
    Move bestMoveFound;

    for (int i = 0; i < moveList.count; i++) {
        pickMove(moveList, i);
        Move move = moveList.moves[i];

        makeMove(move);

        int score;
        if (i == 0) {
            score = -alphaBeta(-beta, -alpha, depthLeft - 1, ply + 1);
        }
        else {
            score = -alphaBeta(-alpha - 1, -alpha, depthLeft - 1, ply + 1);
            if (score > alpha && score < beta) {
                score = -alphaBeta(-beta, -alpha, depthLeft - 1, ply + 1);
            }
        }

        unMakeMove(move, posInfo);

        if (score > bestScore) {
            bestScore = score;
            bestMoveFound = move;
        }

        if (score >= beta) {
            ttEntry->key = key;
            ttEntry->score = beta;
            ttEntry->depth = depthLeft;
            ttEntry->flags = HASH_BETA;
            ttEntry->bestMove = move;

            if (!move.isCapture()) {
                killerMoves[ply][1] = killerMoves[ply][0];
                killerMoves[ply][0] = move;
                historyHeuristic[move.getPieceColor()][move.getTo()][move.getFrom()] += depthLeft * depthLeft;
            }
            return beta;
        }

        if (score > alpha) alpha = score;
    }

    ttEntry->key = key;
    ttEntry->score = alpha;
    ttEntry->depth = depthLeft;
    ttEntry->flags = (alpha > startAlpha) ? HASH_EXACT : HASH_ALPHA;
    ttEntry->bestMove = bestMoveFound;

    return alpha;
}

Move Board::searchPosition()
{
    float timeLimit = 1500.0f;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    Move bestMove;
    int alpha = -INF;
    int beta = INF;

    std::memset(killerMoves, 0, sizeof(killerMoves));
    std::memset(historyHeuristic, 0, sizeof(historyHeuristic));

    for (int depth = 1; depth <= MAX_DEPTH; depth++) {
        int score = alphaBeta(alpha, beta, depth, 0);

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() >= timeLimit) {
            break;
        }

        U64 key = currentHash;
        HashEntry* ttEntry = &transTable[key & (TABLE_SIZE - 1)];
        if (ttEntry->key == key && !ttEntry->bestMove.isNone()) {
            bestMove = ttEntry->bestMove;
            std::cout << "Depth: " << depth << " | Score: " << score << " | Best Move: " << bestMove.getStr() << "\n";
        }
    }
    return bestMove;
}