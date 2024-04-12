#pragma once

const int EN_PASSANT = 1;
const int DOUBLE_PUSH = 2;
const int KING_CASTLING = 3;
const int QUEEN_CASTLING = 4;

class Move
{
	unsigned int move = 0;

public:
	Move() {};
	Move(int from, int to, int piece, int capturedPiece, int promotion, int specialMove);
	int getFrom();
	int getTo();
	int getPiece();
	int getPieceColor();
	int getCapturedPiece();
	int getCaptureColor();
	int getPromotion();
	int isCastling();
	int isDoublePawnPush();
	bool isCapture();
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
