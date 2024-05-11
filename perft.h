#pragma once
#include <chrono>
#include "board.h"
#include <thread>
//https://medium.com/codex/c-multithreading-the-simple-way-95aa1f7304a2 https://www.tutorialspoint.com/cplusplus/cpp_multithreading.htm
//https://www.chessprogramming.org/Checks_and_Pinned_Pieces_(Bitboards)
//https://www.chessprogramming.org/index.php?title=Bitboards&mobileaction=toggle_view_desktop
//https://www.chessprogramming.org/Perft_Results
//https://core.ac.uk/download/572627675.pdf
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

int getPerftWithTime(Board& board, int depth)
{
	auto start = high_resolution_clock::now();
	long long nodes = Perft(board, depth);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	std::cout << "Nodes: " << nodes << "   Time: " << duration.count() / 1000.0 << "\n";
	return nodes;
}