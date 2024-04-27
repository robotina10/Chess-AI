#pragma once
#include "board.h"

int Perft(Board& board, int depth)
{
	if (depth == 0)
		return 1;

	int count = 0;
	MoveList moveList(218);
	board.generateLegalMoves(moveList);

	for (int i = 0; i < moveList.count; i++) {
		board.makeMove(moveList.moves[i]);
		count += Perft(board, depth - 1);
		board.unMakeMove(moveList.moves[i]);
	}
	return count;
}