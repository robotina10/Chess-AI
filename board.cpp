#pragma once
#include "board.h" 
#include "pawnMoves.h"
#include "knightMoves.h"
#include "kingMoves.h"
#include "slidingPieceMoves.h"

U64 northOne(U64 b) { return b >> 8; }
U64 southOne(U64 b) { return b << 8; }
U64 eastOne(U64 b) { return (b & notHFile) << 1; }
U64 noEaOne(U64 b) { return (b & notHFile) >> 7; }
U64 soEaOne(U64 b) { return (b & notHFile) << 9; }
U64 westOne(U64 b) { return (b & notAFile) >> 1; }
U64 soWeOne(U64 b) { return (b & notAFile) << 7; }
U64 noWeOne(U64 b) { return (b & notAFile) >> 9; }


int bitScanForward(U64 bb) { // if !bb -> return 63
	static const int index64[64] = {
		0, 47,  1, 56, 48, 27,  2, 60,
	   57, 49, 41, 37, 28, 16,  3, 61,
	   54, 58, 35, 52, 50, 42, 21, 44,
	   38, 32, 29, 23, 17, 11,  4, 62,
	   46, 55, 26, 59, 40, 36, 15, 53,
	   34, 51, 20, 43, 31, 22, 10, 45,
	   25, 39, 14, 33, 19, 30,  9, 24,
	   13, 18,  8, 12,  7,  6,  5, 63
	};

	const U64 debruijn64 = 0x03f79d71b4cb0a89ULL;
	return index64[((bb ^ (bb - 1)) * debruijn64) >> 58];
}

int bitScanForwardWithReset(U64 &bb) {
	int idx = bitScanForward(bb);
	bb &= bb - 1; // reset bit outside
	return idx;
}

bool Board::attacked(int to, bool side)
{
	if (!side) {
		if (knightAttacks[to] & bb[wKnight]) return true;
		if (pawnAttacks[~side][to] & bb[wPawn]) return true;
		if () return true;
		if () return true;
	}
	else {

	}
}

int Board::generateLegalMoves(MoveList& moveList)
{
	if (whiteTurn) {
		getWhitePawnMoves(moveList);
		getKnightMoves(wKnight, moveList);
		getKingMoves(wKing, moveList);
		getRookMoves(wRook, moveList);
		getBishopMoves(wBishop, moveList);
		getQueenMoves(wQueen, moveList);
	}
	else {
		getBlackPawnMoves(moveList);
		getKnightMoves(bKnight, moveList);
		getKingMoves(bKing, moveList);
		getRookMoves(bRook, moveList);
		getBishopMoves(bBishop, moveList);
		getQueenMoves(bQueen, moveList);
	}
	return moveList.count;
}

void Board::makeMove(Move move)
{
	U64 from = 1ULL << move.getFrom();
	U64 to = 1ULL << move.getTo();
	U64 fromTo = from | to;
	bb[move.getPiece()] ^= fromTo;
	bb[move.getPieceColor()] ^= fromTo;
	if (move.isCapture()) {
		bb[move.getCapturedPiece()] ^= to;
		bb[move.getCaptureColor()] ^= to;
		occupied ^= from;
	}
	else
		occupied ^= fromTo;
	changeTurn();

}

void Board::unMakeMove(Move move)
{
	makeMove(move);
}

void Board::initAttackArrs()
{
	initPawnAttacks();
	initKnightAttacks();
	initKingAttacks();
	initSlidingMasks();
}

U64 Board::getEmpty() { return ~occupied; };
U64 Board::getOccupied() { return occupied; };
U64 Board::getBitboard(int index) { return bb[index]; }
U64 Board::getBitboard(Pieces index) { return bb[index]; }
U64 Board::getEnemy(Pieces piece) { return bb[(piece < 6) ? Blacks : Whites]; }
bool Board::isWhiteTurn() { return whiteTurn; }
bool Board::isEmpty(Pieces piece) { return piece == EMPTY; }
int Board::getCastlingRights() { return castlingRights; }
int Board::getEnPassantSquare() { return enPassantSquare; }

void Board::setWhiteTurn(bool turn) { whiteTurn = turn; }
void Board::changeTurn() { whiteTurn = !whiteTurn; }
void Board::setEnPassantSquare(int shift) { enPassantSquare <= shift; }
void Board::setCastlingRights(CastlingRights right) { castlingRights += right; } // better way by setting invidual bits
void Board::removeCastlingRights(CastlingRights right) { castlingRights -= right; }

Pieces Board::getPiece(int pos)
{
	if ((bb[Whites] & (1ULL << pos)) >> pos) {
		for (int i = 0; i < 6; i++) {
			if ((bb[i] & (1ULL << pos)))
				return (Pieces)i;
		}
	}
	if ((bb[Blacks] & (1ULL << pos)) >> pos) {
		for (int i = 6; i < 12; i++) {
			if ((bb[i] & (1ULL << pos)))
				return (Pieces)i;
		}
	}
	return EMPTY;
}

void Board::placePiece(Pieces piece, int pos)
{
	bb[piece] |= 1ULL << pos;
	bb[(piece < 6) ? Whites : Blacks] |= 1ULL << pos;
	occupied ^= 1ULL << pos;
}

void Board::removePiece(Pieces piece, int pos)
{
	bb[piece] ^= 1ULL << pos;
	bb[(piece < 6) ? Whites : Blacks] ^= 1ULL << pos;
	occupied ^= 1ULL << pos;
}

void Board::setBoard(std::string FEN)
{
	int rank = 0;
	int file = 0;
	int i = 0;
	for (char c: FEN) {
		i++;
		if (c == ' ')
			break;

		int pos = rank * 8 + file;
		if (isdigit(c)) {
			file += atoi(&c);
			continue;
		}

		switch (c) {
		case 'k':
			placePiece(bKing, pos);
			break;
		case 'K':
			placePiece(wKing, pos);
			break;
		case 'q':
			placePiece(bQueen, pos);
			break;
		case 'Q':
			placePiece(wQueen, pos);
			break;
		case 'r':
			placePiece(bRook, pos);
			break;
		case 'R':
			placePiece(wRook, pos);
			break;
		case 'b':
			placePiece(bBishop, pos);
			break;
		case 'B':
			placePiece(wBishop, pos);
			break;
		case 'n':
			placePiece(bKnight, pos);
			break;
		case 'N':
			placePiece(wKnight, pos);
			break;
		case 'p':
			placePiece(bPawn, pos);
			break;
		case 'P':
			placePiece(wPawn, pos);
			break;
		case '/':
			rank++;
			file = -1;
			break;
		}
		file++;
	}
	return;
	for (char c: std::string(FEN.begin() + i, FEN.end())) {
		if (c == 'w') {
			setWhiteTurn(true);
		}
		else if (c == 'b') {
			setWhiteTurn(false);
		}
		else if (c == 'k') {
			setCastlingRights(bKingSide);
		}
		else if (c == 'K') {
			setCastlingRights(wKingSide);
		}
		else if (c == 'Q') {
			setCastlingRights(bQueenSide);
		}
		else if (c == 'q') {
			setCastlingRights(wQueenSide);
		}
		else if (isalpha(c) && isdigit(++c)) {
			c--;
			setEnPassantSquare(c - 97);
		}
		/*else if ("halfMoveclock int") {

		}
		else if ("fullMoveClock int") {

		}*/
	}
}

void Board::printBitboard(int index)
{
	for (int i = 0; i < 64; i++) {
		((bb[index] >> i) & 1ULL) ? std::cout << 1 << " " : std::cout << 0 << " ";
		if ((i + 1) % 8 == 0) {
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}

void Board::printBitboard(U64 bb)
{
	for (int i = 0; i < 64; i++) {
		((bb >> i) & 1ULL) ? std::cout << 1 << " " : std::cout << 0 << " ";
		if ((i + 1) % 8 == 0) {
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}

void Board::printMoves(MoveList& moves) {
	for (Move* move = moves.moves; move != moves.moves + moves.count; move++) {
		std::cout << move->getFrom() << " " << move->getTo() << "\n";
	}
	std::cout << moves.count << "\n";
}
