#pragma once
#include <string>
#include <map>
#include <vector>
#include "constants.h"
#include "move.h"

enum State { DRAW, WHITE_WIN, BLACK_WIN, PLAYING };
enum CastlingRights { wKingSide = 1, wQueenSide = 2, bKingSide = 4, bQueenSide = 8 };

struct CheckingPieces {
	int count = 0;
	Piece piece;
	U64 bb = 0;
};

struct PinnedPieces
{
	U64 all = 0;
	std::map<U64, U64> map;
};

typedef struct Line
{
	int moveCount;
	Move moveLine[lineSize];
};

const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

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
int bitScanForwardWithReset(U64& bb);

extern U64 knightAttacks[64];

// Counts the number of set bits (1s) in a 64-bit integer
inline int popCount(U64 x) {
	x -= (x >> 1) & 0x5555555555555555;
	x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
	return (x * 0x0101010101010101) >> 56;
}

class Board {
	U64 bb[14]{};
	int piecesCount[12]{};
	U64 occupied = 0;

	int castlingRights = 0;
	int enPassantSquare = 0;
	bool whiteTurn = true;

	int halfMoveClock = 0;
	int fullMoveCounter = 1;

	std::vector<U64> history;
	U64 currentHash = 0;

	void getPawnPushCapturesMoves(int color, MoveList& moveList, U64 pawns, U64 empty, U64 checkingPieces, bool capturesOnly);
	void getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Piece piece, int captureDistance);
	void getMovesFromPushBB(MoveList& moveList, U64 bb, Piece piece, int pushDistance, SpecialMove doublePush);
	void getEnPassantMoves(MoveList& moveList, U64 bb, Piece piece, int dir);
	void removeRooksCastlingRights(int rook, int pos);
	void getKnightMoves(Piece knight, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	void getKingMoves(Piece king, MoveList& moveList, bool capturesOnly);
	void getRookMoves(Piece rook, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	void getBishopMoves(Piece bishop, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	void getQueenMoves(Piece queen, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	void getCastlingMoves(Piece king, MoveList& moveList);
	void getWhitePawnMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	void getBlackPawnMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	void generateWhiteMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	void generateBlackMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly);
	U64 rookAttack(int from, U64 occupied);
	U64 bishopAttack(int from, U64 occupied);
	bool attacked(int to, bool side);
	U64 checkedSquares(bool side);
	U64 xrayRookAttacks(U64 occ, U64 blockers, int rookSq);
	U64 xrayBishopAttacks(U64 occ, U64 blockers, int bishopSq);
	void findPinnedPieces(PinnedPieces& pinnedPieces, bool side);
	U64 getPieceAttackingKingDirectAttack(Piece piece, U64 pieceBB);
	void findCheckingPieces(CheckingPieces& cp, bool side);
	void initAttackArrs();
	void setBoard(std::string FEN = defaultFEN);
	void placePiece(Piece piece, int pos);
	U64 getEmpty();
	U64 getEnemy(Piece piece);
	int getCastlingRight(CastlingRights right);
	void changeTurn();
	void removeCastlingRight(CastlingRights right);
	void setCastlingRight(CastlingRights right);

	int alphaBeta(int alpha, int beta, int depthLeft, int maxDepth);
	int QuiescenceSearch(int alpha, int beta);
	int eval();
	void scoreMoves(MoveList& moveList, int depth, Move ttMove = Move());
	void fillZobristArrs();
	U64 ZobristKey();
	void initTables();
public:
	Board copy(); 
	void init();

	void printBB(U64 bb);
	bool isEmpty(int pos);
	Piece getPiece(int pos);
	void makeMove(Move move);
	void unMakeMove(Move move, PosInfo posInfo);
	void setWhiteTurn(bool turn);
	U64 getBitboard(int index);
	U64 getBitboard(Piece index);
	PosInfo getPosInfo();
	bool isWhiteTurn();
	bool isCheck();
	bool isCheckmate(int moveListCount);
	bool isDraw(int moveListCount);
	bool isStalemate(int moveListCount);
	bool isDrawByMaterial();
	bool fiftyMoveRule();
	bool threeFoldRepetitionRule();
	int generateLegalMoves(MoveList& moveList, bool capturesOnly = false);

	std::string saveBoardToFen();

	Move searchPosition();
};