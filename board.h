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

const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
//const std::string defaultFEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "; // perft 5: 193690690

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
	int piecesCount[12]{};
	U64 occupied = 0;

	int castlingRights = 0;
	int enPassantSquare = 0;
	bool whiteTurn = true;

	int halfMoveClock = 0;
	int fullMoveCounter = 1;

	void getPawnPushCapturesMoves(int color, MoveList& moveList, U64 pawns, U64 empty, U64 checkingPieces);
	void getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Pieces piece, int captureDistance);
	void getMovesFromPushBB(MoveList& moveList, U64 bb, Pieces piece, int pushDistance, SpecialMove doublePush);
	void getEnPassantMoves(MoveList& moveList, U64 bb, Pieces piece, int dir);
	void removeRooksCastlingRights(int rook, int pos);
	void getKnightMoves(Pieces knight, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	void getKingMoves(Pieces king, MoveList& moveList);
	void getRookMoves(Pieces rook, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	void getBishopMoves(Pieces bishop, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	void getQueenMoves(Pieces queen, MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	void getCastlingMoves(Pieces king, MoveList& moveList);
	void getWhitePawnMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	void getBlackPawnMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	void generateWhiteMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	void generateBlackMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces);
	U64 rookAttack(int from, U64 occupied);
	U64 bishopAttack(int from, U64 occupied);
	bool attacked(int to, bool side);
	U64 checkedSquares(bool side);
	U64 xrayRookAttacks(U64 occ, U64 blockers, int rookSq);
	U64 xrayBishopAttacks(U64 occ, U64 blockers, int bishopSq);
	void findPinnedPieces(PinnedPieces& pinnedPieces, bool side);
	U64 getPieceAttackingKingDirectAttack(Pieces piece, U64 pieceBB);
	void findCheckingPieces(CheckingPieces& cp, bool side);
	void initAttackArrs();
	void setBoard(std::string FEN = defaultFEN);
	void placePiece(Pieces piece, int pos);
	U64 getEmpty();
	U64 getEnemy(Pieces piece);
	int getCastlingRight(CastlingRights right);
	void changeTurn();
	void removeCastlingRight(CastlingRights right);
	void setCastlingRight(CastlingRights right);
public:
	Board copy(); //copy of itself
	void init();

	void printBB(U64 bb);
	bool isEmpty(Pieces piece);
	Pieces getPiece(int pos);
	void makeMove(Move move);
	void unMakeMove(Board board);
	void setWhiteTurn(bool turn);
	U64 getBitboard(int index);
	U64 getBitboard(Pieces index);
	bool isWhiteTurn();
	bool isCheck();
	bool isCheckmate(int moveListCount);
	bool isDraw(int moveListCount);
	bool isStalemate(int moveListCount);
	bool isDrawByMaterial();
	bool fiftyMoveRule();
	bool threeFoldRepetitionRule();
	int generateLegalMoves(MoveList& moveList);

	std::string saveBoardToFen();

	int negaMax(int depth);
	Move rootNegaMax(int depth);
	int evaluate();
	int countMaterial(int color);
};