#pragma once
#include "constants.h"

enum SpecialMove { NONE, EN_PASSANT, KING_CASTLING, QUEEN_CASTLING, QUEEN_PROM, ROOK_PROM, BISHOP_PROM, KNIGHT_PROM, DOUBLE_PUSH };

class Move
{
	unsigned int move = 0;

public:
	Move() {};
	Move(int from, int to, int piece, int capturedPiece, int specialMove);
	int getFrom();
	int getTo();
	int getPiece();
	int getPieceColor();
	int getPieceGroup();
	int getCapturedPiece();
	int getCaptureGroup();
	int getSpecialMove();
	bool isCapture();
	bool isPromotion();
	bool isCastling();
	void setPromotion(int promotionPiece);
	
	void printMove();
};

struct MoveList
{
	Move *moves;
	int count = 0;
	int size;
	MoveList(int size) : size(size), moves(new Move[size]) {}
	~MoveList() { delete[] moves; }
};
