#pragma once
#include <string>
#include <map>
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

//const std::string defaultFEN = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
//const std::string defaultFEN = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
//const std::string defaultFEN = "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 ";

const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
//const std::string defaultFEN = "4k/4r/4r/8/8/4K/8/8 w - 0 1";
//const std::string defaultFEN = "8/k/3p/p2P1p/P2P1P/8/8/K w - 0 1";
//const std::string defaultFEN = "8/k5P/3p/p2P/P2P1P/8/8/K w - 0 1";
//const std::string defaultFEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "; // perft 5: 193690690 6: 8031647685
//const std::string defaultFEN = "4k3/8/8/8/q/8/PPP4/4K3 w - 0 1";

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
	int piecesCount[12]{}; //do i use it, why did i wanted to use it??? xd :)
	U64 occupied = 0;

	int castlingRights = 0;
	int enPassantSquare = 0;
	bool whiteTurn = true;

	int halfMoveClock = 0;
	int fullMoveCounter = 1;

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
	int quiesce(int alpha, int beta);
	int eval();
	void scoreMoves(MoveList& moveList, int depth);
	//U64 getLeastValuablePiece(U64 attadef, int side, int& piece);
	//int see(int to, Piece target, int from, Piece attacker); //Static Exchange Evaluation
	void fillZobristArrs();
	U64 ZobristKey();
	void recordHash(int val, int hashFlag, int depth);
	int retrieveHash(int alpha, int beta, int depth);
	void initTables();
public:
	Board copy(); //copy of itself
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
	int generateLegalMoves(MoveList& moveList, bool capturesOnly=false);

	std::string saveBoardToFen();

	Move searchPosition();
};