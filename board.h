#pragma once
#include "constants.h"
#include "move.h"

enum CastlingRights { wKingSide = 1, wQueenSide = 2, bKingSide = 4, bQueenSide = 8 };

struct CheckingPieces {
	int count = 0;
	Pieces piece;
	U64 bb;
};

//const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string defaultFEN = "k//8/8/8/8//K3R2q w KQkq - 0 1";

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
	void getMovesFromPushBB(MoveList& moveList, U64 bb, Pieces piece, int pushDistance, SpecialMove doublePush);
	void getEnPassantMoves(MoveList& moveList, U64 bb, Pieces piece, int dir);

public:
	void setBoard(std::string FEN = defaultFEN);
	Pieces getPiece(int pos);
	void initAttackArrs();

	void getWhitePawnMoves(MoveList& moveList, CheckingPieces checkingPieces);
	void getBlackPawnMoves(MoveList& moveList, CheckingPieces checkingPieces);
	void getKnightMoves(Pieces knight, MoveList& moveList, CheckingPieces checkingPieces);
	void getKingMoves(Pieces king, MoveList& moveList);
	void getRookMoves(Pieces rook, MoveList& moveList, CheckingPieces checkingPieces);
	void getBishopMoves(Pieces bishop, MoveList& moveList, CheckingPieces checkingPieces);
	void getQueenMoves(Pieces queen, MoveList& moveList, CheckingPieces checkingPieces);
	void getCastlingMoves(Pieces king, MoveList& moveList);
	bool attacked(int to, bool side);
	bool inCheck(int to, bool side);
	U64 xrayRookAttacks(U64 occ, U64 blockers, int rookSq);
	U64 xrayBishopAttacks(U64 occ, U64 blockers, int bishopSq);
	U64 absolutePins(bool side);
	//U64 getEnemyAttack(bool side);
	U64 getPieceAttack(Pieces piece, int from);
	void findCheckingPieces(CheckingPieces &cp, bool side);
	void generateWhiteMoves(MoveList& moveList, CheckingPieces checkingPieces);
	void generateBlackMoves(MoveList& moveList, CheckingPieces checkingPieces);
	int generateLegalMoves(MoveList& moveList);

	void placePiece(Pieces piece, int pos);
	void makeMove(Move move);
	void unMakeMove(Move move);

	U64 getEmpty();
	U64 getBitboard(int index);
	U64 getBitboard(Pieces index);
	U64 getEnemy(Pieces piece);
	bool isWhiteTurn();
	bool isEmpty(Pieces piece);
	int getEnPassantSquare();
	int getCastlingRight(CastlingRights right);

	void setWhiteTurn(bool turn);
	void changeTurn();
	void setCastlingRight(CastlingRights right);

	void printBitboard(U64 bb);
	void printMoves(MoveList& moves);
};