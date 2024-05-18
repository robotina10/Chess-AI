#pragma once
#include <chrono>
#include <thread>
#include "board.h"

using namespace std::chrono;

long long Perft(Board& board, int depth)
{
	if (depth == 0)
		return 1;

	long long count = 0;
	MoveList moveList(218);
	board.generateLegalMoves(moveList);

	for (int i = 0; i < moveList.count; i++) {
		board.makeMove(moveList.moves[i]);
		count += Perft(board, depth - 1);
		board.unMakeMove(moveList.moves[i]);
	}
	return count;
}

long long getPerftWithTime(Board& board, int depth)
{
	auto start = high_resolution_clock::now();
	long long nodes = Perft(board, depth);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << "Nodes: " << nodes << "   Time: " << duration.count() / 1000.0 << "\n";
	return nodes;
}