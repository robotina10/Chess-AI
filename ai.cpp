#include "board.h"

const int pawnVal = 100;
const int knightVal = 300;
const int bishopVal = 300;
const int rookVal = 500;
const int queenVal = 900;

int Board::generateLegalCapturesOnly(MoveList& moveList)
{
	CheckingPieces checkingPieces;
	findCheckingPieces(checkingPieces, whiteTurn);
	PinnedPieces pinnedPieces;
	findPinnedPieces(pinnedPieces, whiteTurn);

	if (!checkingPieces.count) {
		checkingPieces.bb = ~0;
		if (whiteTurn)
			generateWhiteMoves(moveList, checkingPieces.bb, pinnedPieces);
		else
			generateBlackMoves(moveList, checkingPieces.bb, pinnedPieces);
	}
	else if (checkingPieces.count == 1) {
		if (whiteTurn) {
			if (checkingPieces.piece == bKnight || checkingPieces.piece == bPawn) {
				generateWhiteMoves(moveList, checkingPieces.bb, pinnedPieces);
			}
			else {
				checkingPieces.bb |= getPieceAttackingKingDirectAttack(checkingPieces.piece, checkingPieces.bb);
				generateWhiteMoves(moveList, checkingPieces.bb, pinnedPieces);
			}
		}
		else {
			if (checkingPieces.piece == wKnight || checkingPieces.piece == wPawn) {
				generateBlackMoves(moveList, checkingPieces.bb, pinnedPieces);
			}
			else {
				checkingPieces.bb |= getPieceAttackingKingDirectAttack(checkingPieces.piece, checkingPieces.bb);
				generateBlackMoves(moveList, checkingPieces.bb, pinnedPieces);
			}
		}
	}
	else {
		getKingMoves((Pieces)(bKing + whiteTurn), moveList);
	}
	return moveList.count;
}

int Board::evaluate()
{
	int whiteEval = countMaterial(White);
	int blackEval = countMaterial(Black);
	int materialScore = whiteEval - blackEval;
	int mobilityScore;
	return (materialScore + mobilityScore) * (whiteTurn) ? 1 : -1;
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

int Board::countMobilityScore(int color)
{

}

void Board::orderMoves(MoveList& moveList)
{
	for (int i = 0; i < moveList.count; i++) {

	}
}

int Board::quiesce(int alpha, int beta)
{
	int stand_pat = evaluate();
	if (stand_pat >= beta)
		return beta;
	if (alpha < stand_pat)
		alpha = stand_pat;

	MoveList moveList(256);
	generateLegalCapturesOnly(moveList);
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

int Board::alphaBeta(int alpha, int beta, int depthLeft)
{
	if (depthLeft == 0) {
		return quiesce(alpha, beta);
	}

	MoveList moveList(256);
	generateLegalMoves(moveList);

	if (!moveList.count) {
		if (isCheck())
			return INT_MIN;
		return 0;
	}

	Board b = copy();

	for (int i = 0; i < moveList.count; i++) {
		makeMove(moveList.moves[i]);
		int score = -alphaBeta(-beta, -alpha, depthLeft - 1);
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
		int n = -alphaBeta(INT_MIN, INT_MAX, depth - 1);
		unMakeMove(b);
		if (n > bestEval) {
			bestEval = n;
			bestMove = moveList.moves[i];
		}
	}
	return bestMove;
}