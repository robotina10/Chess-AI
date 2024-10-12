#pragma once
#include <chrono>
#include <thread>
#include "board.h"

using namespace std::chrono;

long long Perft(Board board, int depth, int maxDepth)
{

	long long count = 0;
	MoveList moveList(256);
	board.generateLegalMoves(moveList);

	if (depth == 1)
		return moveList.count;

	Board b = board.copy();

	for (int i = 0; i < moveList.count; i++) {
		board.makeMove(moveList.moves[i]);
		long long c = Perft(board, depth - 1, maxDepth);
		if (depth == maxDepth) {
			moveList.moves[i].printMove();
			std::cout << c << "\n";
		}
		count += c;
		//count += Perft(board, depth - 1, maxDepth);
		board.unMakeMove(b);
	}
	return count;
}

void getPerftWithTime(Board board, int maxDepth)
{ 
	/*for (int d = 1; d <= maxDepth; d++) {
		long long nodes = Perft(board, d);
		std::cout << "Nodes: " << nodes << "   Depth: " << d << "\n";
	}*/

	auto start = high_resolution_clock::now();
	long long nodes = Perft(board, maxDepth, maxDepth);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << "Nodes: " << nodes << "   Time: " << duration.count() / 1000.0 << "\n";
	std::cout << std::fixed << std::cout.precision(3) << "  " << (nodes / (duration.count() / 1000.0)) / 1000000 << " Mn/s" << "\n"; // in output is 6 wtf?

}