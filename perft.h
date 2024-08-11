#pragma once
#include <chrono>
#include <thread>
#include "board.h"

using namespace std::chrono;

int ep = 0;
int checks = 0;
int captures = 0;
int checkmates = 0;
int promotions = 0;
int castles = 0;


long long Perft(Board& board, int depth, int maxDepth)
{
	if (depth == 0) {
		return 1;
	}

	long long count = 0;
	MoveList moveList(256);
	board.generateLegalMoves(moveList);
	Board b = board;

	for (int i = 0; i < moveList.count; i++) {
		board.makeMove(moveList.moves[i]);
		if (depth == 1) {
			if (moveList.moves[i].isCapture())
				captures++;
			if (moveList.moves[i].getSpecialMove() == EN_PASSANT)
				ep++;
			if (moveList.moves[i].isPromotion())
				promotions++;
			if (moveList.moves[i].isCastling())
				castles++;
			if (board.isCheck()) {
				checks++;
				MoveList ml(256);
				board.generateLegalMoves(ml);
				if (!ml.count)
					checkmates++;
			}
		}
		int c = Perft(board, depth - 1, maxDepth);
		if (depth == maxDepth) {
			moveList.moves[i].printMove();
			std::cout << c << "\n";
		}
		count += c;
		//count += Perft(board, depth - 1);

		board = b;
	}
	return count;
}

void getPerftWithTime(Board& board, int maxDepth)
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
	std::cout << "E.p.: " << ep << "\n";
	std::cout << "Captures: " << captures << "\n";
	std::cout << "Checkmates: " << checkmates << "\n";
	std::cout << "Checks: " << checks << "\n";
	std::cout << "Promotions: " << promotions << "\n";
	std::cout << "Castles: " << castles << "\n";
}