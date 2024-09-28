#pragma once
#include <string>
#include <map>
#include "constants.h"
#include "move.h"

enum State { DRAW, WHITE_WIN, BLACK_WIN, PLAYING };
enum CastlingRights { wKingSide = 1, wQueenSide = 2, bKingSide = 4, bQueenSide = 8 };

struct CheckingPieces {
	int count = 0;
	Pieces piece;
	U64 bb = 0;
};

struct PinnedPieces
{
	U64 all = 0;
	std::map<U64, U64> map;
};

//const std::string defaultFEN = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
const std::string defaultFEN = "r3k2r/Pp1p1ppp/1bp2nbN/nPP5/BB2P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 2";
//const std::string defaultFEN = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ";
//const std::string defaultFEN = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ";
//const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
//const std::string defaultFEN = "rnbqkbnr/ppp2ppp/8/8/8/8/PPP2PPP/RNBQKBNR w KQkq - 0 1";
//const std::string defaultFEN = "3k3/5p/8/q5PK/8/8/8/8 b - 0 1";
//const std::string defaultFEN = "k//8/8/8/8//K3R2q w KQkq - 0 1";
//const std::string defaultFEN = "k///q//8//K3B2 w KQkq - 0 1";

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

	int halfMoveClock = 0;
	int fullMoveCounter = 1;

	void getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Pieces piece, int captureDistance);
	void getMovesFromPushBB(MoveList& moveList, U64 bb, Pieces piece, int pushDistance, SpecialMove doublePush);
	void getEnPassantMoves(MoveList& moveList, U64 bb, Pieces piece, int dir);

public:
	void setBoard(std::string FEN = defaultFEN);
	Pieces getPiece(int pos);
	void initAttackArrs();

	bool isCheck();
	bool isCheckmate(int moveListCount);
	bool isStalemate(int moveListCount);
	bool isDrawByMaterial();
	bool fiftyMoveRule();
	void getWhitePawnMoves(MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	void getBlackPawnMoves(MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	void getKnightMoves(Pieces knight, MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	void getKingMoves(Pieces king, MoveList& moveList);
	void getRookMoves(Pieces rook, MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	void getBishopMoves(Pieces bishop, MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	void getQueenMoves(Pieces queen, MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	void getCastlingMoves(Pieces king, MoveList& moveList);
	U64 rookAttack(int from, U64 occupied);
	U64 bishopAttack(int from, U64 occupied);
	bool attacked(int to, bool side);
	U64 checkedSquares(bool side);
	U64 xrayRookAttacks(U64 occ, U64 blockers, int rookSq);
	U64 xrayBishopAttacks(U64 occ, U64 blockers, int bishopSq);
	void findPinnedPieces(PinnedPieces& pinnedPieces, bool side);
	U64 getPieceAttackingKingDirectAttack(Pieces piece, U64 pieceBB);
	void findCheckingPieces(CheckingPieces &cp, bool side);
	void generateWhiteMoves(MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	void generateBlackMoves(MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces);
	int generateLegalMoves(MoveList& moveList);

	void placePiece(Pieces piece, int pos);
	void makeMove(Move move);

	U64 getEmpty();
	U64 getBitboard(int index);
	U64 getBitboard(Pieces index);
	U64 getEnemy(Pieces piece);
	bool isWhiteTurn();
	bool isEmpty(Pieces piece);
	int getCastlingRight(CastlingRights right);

	void setWhiteTurn(bool turn);
	void changeTurn();
	void removeCastlingRight(CastlingRights right);
	void setCastlingRight(CastlingRights right);

	std::string saveBoardToFen();
	void printBB(U64 bb);
};