#pragma once
#include <chrono>
#include "board.h"

using namespace std::chrono;

long long Perft(Board board, int depth, int maxDepth)
{
	long long count = 0;
	MoveList moveList(256);
	board.generateLegalMoves(moveList);
	//for (int i = 0; i < moveList.count; i++) {moveList.moves[i].printMove();std::cout << "1" << "\n";}
	if (depth == 1)
		return moveList.count;

	PosInfo posInfo = board.getPosInfo();
	//Board b = board.copy();
	for (int i = 0; i < moveList.count; i++) {
		board.makeMove(moveList.moves[i]);
		long long c = Perft(board, depth - 1, maxDepth);
		board.unMakeMove(moveList.moves[i], posInfo);
		//board = b;
		if (depth == maxDepth) {
			
			//std::cout << moveList.moves[i].getStr() << " :" << c << "\n";
		}
		count += c;
		//count += Perft(board, depth - 1, maxDepth);
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

void perftTestDefaultFen()
{
	Board board;
	board.init();
	long long nodes[7] = { 20, 400, 8902, 197281, 4865609, 119060324, 3195901860 };
	for (int depth = 1; depth <= 7; depth++) {
		long long count = Perft(board, depth, depth);
		if (count == nodes[depth - 1]) {
			std::cout << "Perft test passed for depth " << depth << ": " << count << " nodes.\n";
		}
		else {
			std::cout << "Perft test FAILED for depth " << depth << ": expected " << nodes[depth - 1] << ", got " << count << ".\n";
		}
	}
}