//#include "ai.h"
#include "board.h"

const int pawnVal = 100;
const int knightVal = 300;
const int bishopVal = 300;
const int rookVal = 500;
const int queenVal = 900;

int Board::evaluate()
{
	int whiteEval = countMaterial(White);
	int blackEval = countMaterial(Black);
	whiteTurn = (whiteTurn) ? 1 : -1;
	return (whiteEval - blackEval) * whiteTurn;
}

int Board::countMaterial(int color) //https://www.chessprogramming.org/Material
{
	int material = piecesCount[bPawn + color] * pawnVal;
	material += piecesCount[bKnight + color] * knightVal;
	material += piecesCount[bBishop + color] * bishopVal;
	material += piecesCount[bRook + color] * rookVal;
	material += piecesCount[bQueen + color] * queenVal;
	return material;
}

int Board::negaMax(int depth)
{
	if (depth == 0) {
		return evaluate();
	}

	MoveList moveList(256);
	generateLegalMoves(moveList);


	if (isCheckmate(moveList.count))
		return INT_MIN;
	else
		return 0;

	int bestEval = INT_MIN;
	Board b = copy();

	for (int i = 0; i < moveList.count; i++) {
		makeMove(moveList.moves[i]);
		int n = -negaMax(depth - 1);
		if (n > bestEval) {
			bestEval = n;
		}
		unMakeMove(b);
	}
	return bestEval;
}

Move Board::rootNegaMax(int depth)
{
	MoveList moveList(256);
	generateLegalMoves(moveList);

	if (!moveList.count) {
		return Move();
	}

	int bestEval = INT_MIN;
	Move bestMove;
	Board b = copy();

	for (int i = 0; i < moveList.count; i++) {
		makeMove(moveList.moves[i]);
		int n = -negaMax(depth - 1);
		if (n > bestEval) {
			bestEval = n;
			bestMove = moveList.moves[i];
		}
		unMakeMove(b);
	}
	return bestMove;
}