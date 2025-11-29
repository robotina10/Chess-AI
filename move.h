#pragma once
#include <string>
#include <array>
#include "constants.h"

enum SpecialMove { NONE, EN_PASSANT, KING_CASTLING, QUEEN_CASTLING, QUEEN_PROM, ROOK_PROM, BISHOP_PROM, KNIGHT_PROM, DOUBLE_PUSH };

class Move
{
	unsigned int move = 0;

public:
	Move() = default;
	Move(int from, int to, int piece, int capturedPiece, int specialMove);
	bool operator==(const Move m) { return m.move == m.move; }
	bool operator!=(const Move m) { return m.move != m.move; }
	int getFrom();
	int getTo();
	int getPiece();
	int getPieceColor();
	int getPieceGroup();
	int getCapturedPiece();
	int getSpecialMove();
	bool isCapture();
	bool isPromotion();
	bool isCastling();
	void setPromotion(int promotionPiece);
	std::string getStr();
};

struct MoveList
{
	int count = 0;
	int size;
	Move* moves;
	int* scores;
	MoveList(int size) : size(size), moves(new Move[size]), scores(new int[size]) {}
	~MoveList() { delete[] moves; delete[] scores; }
};

/*template <size_t N> // stack allocated (probably not enough size in stack)
struct MoveList {
	int count = 0;
	std::array<Move, N> moves; 
	std::array<int, N> scores;  

	MoveList() = default;       

	MoveList(int size) : count(size) {}
};*/

struct PosInfo
{
	unsigned int posInfo;
	int getCastlingRights();
	int getEpSquare();
	int getHalfMoveClock();
	PosInfo() = default;
	PosInfo(int castlingRights, int epSquare, int halfMoveClock);
};

struct Position
{
	Move move;
	PosInfo info;
	Position() = default;
	Position(Move move, PosInfo info) : move(move), info(info) {}
};
