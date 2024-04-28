#pragma once
#include <iostream>
#include "move.h"

typedef std::uint64_t U64;

enum Pieces {wKing, wQueen, wRook, wBishop, wKnight, wPawn, bKing, bQueen, bRook, bBishop, bKnight, bPawn, Whites, Blacks, EMPTY };
enum CastlingRights { wKingSide = 1, wQueenSide = 2, bKingSide = 4, bQueenSide = 8 };

//const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string defaultFEN = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";

const U64 notAFile = ~0x0101010101010101;
const U64 notHFile = ~0x8080808080808080;

U64 southOne(U64 b);
U64 northOne(U64 b);
U64 eastOne(U64 b);
U64 noEaOne(U64 b);
U64 soEaOne(U64 b);
U64 westOne(U64 b);
U64 soWeOne(U64 b);
U64 noWeOne(U64 b);

int bitScanForward(U64 bb);
int bitScanForwardWithReset(U64 &bb);

class Board {
	U64 bb[14]{};
	U64 occupied = 0;

	int castlingRights = 0;
	int enPassantSquare = 0;
	bool whiteTurn = true;

	void getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Pieces piece, int captureDistance);
	void getMovesFromPushBB(MoveList& moveList, U64 bb, Pieces piece, int pushDistance);

public:
	void setBoard(std::string FEN = defaultFEN);
	Pieces getPiece(int pos);
	void initAttackArrs();

	void getWhitePawnMoves(MoveList& moveList);
	void getBlackPawnMoves(MoveList& moveList);
	void getKnightMoves(Pieces knight, MoveList& moveList);
	void getKingMoves(Pieces king, MoveList& moveList);
	void getRookMoves(Pieces rook, MoveList& moveList);
	void getBishopMoves(Pieces bishop, MoveList& moveList);
	void getQueenMoves(Pieces queen, MoveList& moveList);
	void getCastlingMoves(Pieces king, MoveList& moveList);
	bool attacked(int to, bool side);
	bool inCheck(int to, bool side);
	bool inCheck(Move move, bool side);
	int generateLegalMoves(MoveList& moveList);

	void placePiece(Pieces piece, int pos);
	void removePiece(Pieces piece, int pos);
	void makeMove(Move move);
	void unMakeMove(Move move);

	U64 getEmpty();
	U64 getOccupied();
	U64 getBitboard(int index);
	U64 getBitboard(Pieces index);
	U64 getEnemy(Pieces piece);
	bool isWhiteTurn();
	bool isEmpty(Pieces piece);
	int getEnPassantSquare();
	int getCastlingRight(CastlingRights right);

	void setWhiteTurn(bool turn);
	void changeTurn();
	void setEnPassantSquare(int shift);
	void setCastlingRight(CastlingRights right);

	void printBitboard(int index);
	void printBitboard(U64 bb);
	void printMoves(MoveList& moves);
};