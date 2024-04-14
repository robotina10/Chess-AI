#pragma once
#include <iostream>
#include "move.h"
/*
a-file             0x0101010101010101
h-file             0x8080808080808080
1st rank           0x00000000000000FF
8th rank           0xFF00000000000000
a1-h8 diagonal     0x8040201008040201
h1-a8 antidiagonal 0x0102040810204080
light squares      0x55AA55AA55AA55AA
dark squares       0xAA55AA55AA55AA55*/
typedef std::uint64_t U64;

enum Pieces {wKing, wQueen, wRook, wBishop, wKnight, wPawn, bKing, bQueen, bRook, bBishop, bKnight, bPawn, Whites, Blacks, EMPTY };

enum CastlingRights { wKingSide, wQueenSide, bKingSide, bQueenSide };

const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const U64 notAFile = 0xfefefefefefefefe; // ~0x0101010101010101
const U64 notHFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080

U64 southOne(U64 b);
U64 northOne(U64 b);
U64 eastOne(U64 b);
U64 noEaOne(U64 b);
U64 soEaOne(U64 b);
U64 westOne(U64 b);
U64 soWeOne(U64 b);
U64 noWeOne(U64 b);

int bitScanForwardWithReset(U64 &bb);


class Board {
	U64 bb[14]{};
	U64 occupied = 0;

	bool whiteTurn;
	int castlingRights = 0;
	int enPassantSquare = 0;

	void getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Pieces piece, int captureDistance);

public:
	void setBoard(std::string FEN = defaultFEN);
	Pieces getPiece(int pos);
	void initAttackArr();

	void getWhitePawnMoves(MoveList& moveList);
	void getBlackPawnMoves(MoveList& moveList);
	void getKnightMoves(Pieces knight, MoveList& moveList);
	int generateLegalMoves(MoveList& moveList);

	void placePiece(Pieces piece, int pos);
	void removePiece(Pieces piece, int pos);
	void makeMove(Move move);
	void unmakeMove(Move move);

	U64 getEmpty();
	U64 getOccupied();
	U64 getBitboard(int index);
	U64 getBitboard(Pieces index);
	bool isWhiteTurn();
	bool isEmpty(Pieces piece);
	int getCastlingRights();
	int getEnPassantSquare();

	void setWhiteTurn(bool turn);
	void setEnPassantSquare(int shift);
	void setCastlingRights(CastlingRights right);
	void removeCastlingRights(CastlingRights right);

	void printBitboard(int index);
	void printMoves(MoveList& moves);
};