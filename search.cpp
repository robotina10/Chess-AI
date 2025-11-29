#include <chrono>
#include "board.h"
#include "transpositionTable.h"

const int MATE_SCORE = -1000000000;
const int MAX_DEPTH = 125;

//most valuable victim, least valuable atacker
int mvv_lva[6][6] = {
	{0, 0, 0, 0, 0, 0} ,       // victim K, attacker K, Q, R, B, N, P,
	{50, 51, 52, 53, 54, 55}, // victim Q, attacker K, Q, R, B, N, P,
	{40, 41, 42, 43, 44, 45}, // victim R, attacker K, Q, R, B, N, P,
	{30, 31, 32, 33, 34, 35,}, // victim B, attacker K, Q, R, B, N, P,
	{20, 21, 22, 23, 24, 25}, // victim N, attacker K, Q, R, B, N, P,
	{10, 11, 12, 13, 14, 15}, // victim P, attacker K, Q, R, B, N, P,
};

const int value[] = { 100, 100, 300, 300, 500, 500, 1000, 1000 };
int movesCount = 0;
Move killerMoves[MAX_DEPTH][2];
int history[2][64][64];

bool timedOut(std::chrono::steady_clock::time_point start, float timeLimit)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() >= timeLimit;
}

/*U64 Board::getLeastValuablePiece(U64 attadef, int side, int& piece)
{
	for (piece = bPawn + side; piece >= bKing + side; piece -= 2) {
		U64 subset = attadef & bb[piece];
		if (subset)
			return subset & -subset; // single bit
	}
	return 0;
}

int Board::see(int to, Piece target, int from, Piece attacker) // https://www.chessprogramming.org/SEE_-_The_Swap_Algorithm
{
	int side = pieceColor::getPieceColor(target);
	int gain[32], d = 0;
	U64 mayXray = bb[bPawn + side] | bb[bBishop + side] | bb[bRook + side] | bb[bQueen + side];
	U64 fromSet = 1ULL << from;
	U64 occ = occupied;
	U64 attadef = attacked(occ, to);
	gain[d] = value[target];
	do {
		d++; // next depth and side
		gain[d] = value[attacker] - gain[d - 1]; // speculative store, if defended
		attadef ^= fromSet; // reset bit in set to traverse
		occ ^= fromSet; // reset bit in temporary occupancy (for x-Rays)
		if (fromSet & mayXray) {
			if ()
				attadef |= xrayBishopAttacks();
			if ()
				attadef |= xrayRookAttacks(occ, );
		}
		fromSet = getLeastValuablePiece(attadef, d & 1, (int &)attacker);
	} while (fromSet);
	while (--d) {
		gain[d - 1] = -std::max(-gain[d - 1], gain[d]);
	}
	return gain[0];
}*/

void storeKillerMove(Move move, int ply)
{
	if (killerMoves[ply][0] != move) {
		killerMoves[ply][1] = killerMoves[ply][0];
		killerMoves[ply][0] = move;
	}
}

void storeHistoryMove(Move move, int ply)
{
	history[move.getPieceColor()][move.getTo()][move.getFrom()] += ply * ply;
}

void Board::scoreMoves(MoveList& moveList, int depth)
{
	//tt
	//mvv-lva, see???
	//killmoves
	//historyheuristic
	const int MVV_LVA_OFFSET = 1000000;
	const int KILLER_VALUE = 900;
	const int HISTORY_VALUE = 100;
	for (int i = 0; i < moveList.count; i++) {
		Move move = moveList.moves[i];
		if (move.isCapture()) {
			moveList.scores[i] = MVV_LVA_OFFSET + mvv_lva[move.getCapturedPiece()][move.getPiece()];
		}
		else {
			if (move == killerMoves[depth][0])
				moveList.scores[i] = MVV_LVA_OFFSET - KILLER_VALUE;
			else if (move == killerMoves[depth][1])
				moveList.scores[i] = MVV_LVA_OFFSET - 2 * KILLER_VALUE;
			else
				moveList.scores[i] = history[move.getPieceColor()][move.getTo()][move.getFrom()] * HISTORY_VALUE;
		}
	}
}

void selectMove(MoveList &moveList, int start)
{
	for (int i = start + 1; i < moveList.count; i++) {
		if (moveList.scores[i] > moveList.scores[start]) {
			std::swap(moveList.scores[start], moveList.scores[i]);
			std::swap(moveList.moves[start], moveList.moves[i]);
		}
	}
}

int Board::quiesce(int alpha, int beta)
{
	int standPat = eval();
	if (standPat >= beta)
		return beta;
	if (alpha < standPat)
		alpha = standPat;

	MoveList moveList(218);
	generateLegalMoves(moveList, true);

	if (!moveList.count) {
		if (isCheck())
			return MATE_SCORE;
		return 0;
	}
	scoreMoves(moveList, 0);

	PosInfo posInfo(castlingRights, enPassantSquare, halfMoveClock);

	for (int i = 0; i < moveList.count; i++) {
		selectMove(moveList, i);
		makeMove(moveList.moves[i]);
		movesCount++;
		int score = -quiesce(-beta, -alpha);
		unMakeMove(moveList.moves[i], posInfo);

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

int Board::alphaBeta(int alpha, int beta, int depthLeft, int maxDepth)
{
	if (depthLeft == 0) {
		 return quiesce(alpha, beta);
	}

	MoveList moveList(218);
	generateLegalMoves(moveList);

	if (!moveList.count) {
		if (isCheck())
			return MATE_SCORE + (maxDepth - depthLeft);
		return 0;
	}
	scoreMoves(moveList, depthLeft);

	PosInfo posInfo(castlingRights, enPassantSquare, halfMoveClock);


	int ply = maxDepth - depthLeft;
	for (int i = 0; i < moveList.count; i++) {
		selectMove(moveList, i);
		Move move = moveList.moves[i];
		makeMove(move);
		movesCount++;
		int score = -alphaBeta(-beta, -alpha, depthLeft - 1, maxDepth);
		unMakeMove(move, posInfo);

		if (score >= beta) {
			if (!move.isCapture()) {
				storeKillerMove(move, ply);
				storeHistoryMove(move, ply);
			}
			return beta;
		}

		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

Move Board::searchPosition()
{ 
	float timeLimit = 1000; //miliseconds
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

	Move bestMove;
	MoveList moveList(218);
	generateLegalMoves(moveList);
	PosInfo posInfo(castlingRights, enPassantSquare, halfMoveClock);
	int depth = 2;
	//for (int depth = 1; depth < MAX_DEPTH; depth++) {
		int bestScore = INT_MIN;
		scoreMoves(moveList, depth);

		for (int i = 0; i < moveList.count; i++) {
			selectMove(moveList, i);
			Move move = moveList.moves[i];
			makeMove(move);
			movesCount++;
			int score = -alphaBeta(-1000000000, 1000000000, depth - 1, depth);
			unMakeMove(move, posInfo);
			std::cout << move.getStr() << ": ";
			std::cout << score << "\n";
			if (score > bestScore) {
				bestScore = score;
				bestMove = move;
			}
			//if (timedOut(start, timeLimit)) {
			//	std::cout << "Depth searched: " << depth << " Moves searched: " << movesCount << "\n";
			//	movesCount = 0;
			//	return bestMove;
			//}
		}
	//}
	std::cout <<  "Moves searched : " << movesCount << "\n";
	return bestMove;
}