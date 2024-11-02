#include "board.h"

const int MATE_SCORE = 100000;

void Board::orderMoves(MoveList& moveList)
{
	return;
	for (int i = 0; i < moveList.count; i++) {


	}
}

int Board::quiesce(int alpha, int beta)
{
	int stand_pat = eval();
	if (stand_pat >= beta)
		return beta;
	if (alpha < stand_pat)
		alpha = stand_pat;

	MoveList moveList(256);
	generateLegalMoves(moveList, true);

	if (!moveList.count) {
		if (isCheck())
			return -MATE_SCORE; //better way? same in alphabeta
		return 0;
	}

	orderMoves(moveList);

	Board b = copy();

	for (int i = 0; i < moveList.count; i++) {
		makeMove(moveList.moves[i]);
		int score = -quiesce(-beta, -alpha);
		unMakeMove(b);

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

int Board::alphaBeta(int alpha, int beta, int depthLeft, int maxDepth) //not copy the board????
{
	if (depthLeft == 0) {
		return quiesce(alpha, beta);
	}

	MoveList moveList(256);
	generateLegalMoves(moveList);

	if (!moveList.count) {
		if (isCheck())
			return -MATE_SCORE + (maxDepth - depthLeft);
		return 0;
	}
	 
	orderMoves(moveList);

	Board b = copy();

	for (int i = 0; i < moveList.count; i++) {
		makeMove(moveList.moves[i]);
		int score = -alphaBeta(-beta, -alpha, depthLeft - 1, maxDepth);
		unMakeMove(b);

		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	return alpha;
}

Move Board::searchRoot(int depth)
{
	MoveList moveList(256);
	generateLegalMoves(moveList);
	orderMoves(moveList);

	int bestEval = INT_MIN;
	Move bestMove;
	Board b = copy();

	for (int i = 0; i < moveList.count; i++) {
		makeMove(moveList.moves[i]);
		int n = -alphaBeta(-INT_MAX, INT_MAX, depth - 1, depth);
		unMakeMove(b); 
		std::cout << n << "\n";
		if (n > bestEval) {
			bestEval = n;
			bestMove = moveList.moves[i];
		}
	}
	bestMove.printMove();
	return bestMove;
}