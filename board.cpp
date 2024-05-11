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
		if (pawnAttacks[0][to] & bb[wPawn]) return true;
		U64 queenRook = bb[wQueen] | bb[wRook];
		if (rookAttack(to, occupied) & queenRook) return true;
		U64 queenBishop = bb[wBishop] | bb[wQueen];
		if (bishopAttack(to, occupied) & queenBishop) return true;
	}
	else {
		if (knightAttacks[to] & bb[bKnight]) return true;
		if (pawnAttacks[1][to] & bb[bPawn]) return true;
		U64 queenRook = bb[bQueen] | bb[bRook];
		if (rookAttack(to, occupied) & queenRook) return true;
		U64 queenBishop = bb[bBishop] | bb[bQueen];
		if (bishopAttack(to, occupied) & queenBishop) return true;
	}
}

U64 Board::attacksToKing(int to, bool side)
{
	if (!side) {

		U64 queenRook = bb[wQueen] | bb[wRook];
		U64 queenBishop = bb[wBishop] | bb[wQueen];
		return knightAttacks[to] & bb[wKnight] 
			| pawnAttacks[0][to] & bb[wPawn] 
			| rookAttack(to, occupied) & queenRook 
			| bishopAttack(to, occupied) & queenBishop;
	}
	else {
		U64 queenRook = bb[bQueen] | bb[bRook];
		U64 queenBishop = bb[bBishop] | bb[bQueen];
		return knightAttacks[to] & bb[bKnight]
			| pawnAttacks[1][to] & bb[bPawn]
			| rookAttack(to, occupied) & queenRook
			| bishopAttack(to, occupied) & queenBishop;
	}
}

bool Board::inCheck(int to, bool side)
{
	int king = wKing;
	if (!side)
		king = bKing;
	occupied ^= bb[king];
	bool check = attacked(to, side);
	occupied ^= bb[king];
	return check;
}

bool Board::inCheck(Move move, bool side)
{
	int ep = enPassantSquare;
	makeMove(move);
	int king = wKing;
	if (!side) 
		king = bKing;
	int to = bitScanForward(bb[king]);
	occupied ^= bb[king];
	bool check = attacked(to, side);
	occupied ^= bb[king];
	unMakeMove(move);
	enPassantSquare = ep;
	return check;
}

int Board::generateLegalMoves(MoveList& moveList)
{
	if (whiteTurn) {
		U64 check = attacksToKing(bitScanForward(bb[wKing]), whiteTurn);
		getWhitePawnMoves(moveList, check);
		getKnightMoves(wKnight, moveList, check);
		getKingMoves(wKing, moveList, check);
		getRookMoves(wRook, moveList, check);
		getBishopMoves(wBishop, moveList, check);
		getQueenMoves(wQueen, moveList, check);
	}
	else {
		U64 check = attacksToKing(bitScanForward(bb[bKing]), whiteTurn);
		getBlackPawnMoves(moveList, check);
		getKnightMoves(bKnight, moveList, check);
		getKingMoves(bKing, moveList, check);
		getRookMoves(bRook, moveList, check);
		getBishopMoves(bBishop, moveList, check);
		getQueenMoves(bQueen, moveList, check);
	}
	return moveList.count;
}

void Board::makeMove(Move move)
{
	int piece = move.getPiece();
	U64 from = 1ULL << move.getFrom();
	U64 to = 1ULL << move.getTo();
	U64 fromTo = from | to;
	bb[piece] ^= fromTo;
	bb[move.getPieceColor()] ^= fromTo;
	if (move.isCapture()) {
		bb[move.getCapturedPiece()] ^= to;
		bb[move.getCaptureColor()] ^= to;
		occupied ^= from;
	}
	else
		occupied ^= fromTo;

	switch (piece) {
	case wKing:
		setCastlingRight(wKingSide);
		setCastlingRight(wQueenSide);
		break;
	case bKing:
		setCastlingRight(bKingSide);
		setCastlingRight(bQueenSide);
		break;
	case wRook:
		if (move.getFrom() == 56)
			setCastlingRight(wQueenSide);
		else if (move.getFrom() == 63)
			setCastlingRight(wKingSide);
		break;
	case bRook:
		if (move.getFrom() == 0)
			setCastlingRight(bQueenSide);
		else if (move.getFrom() == 7)
			setCastlingRight(bKingSide);
		break;
	}

	switch (move.getSpecialMove()) {
	case DOUBLE_PUSH:
		enPassantSquare = move.getTo();
		break;
	case KING_CASTLING:
		from <<= 3;
		to >>= 1;
		fromTo = from | to;
		bb[(piece < 6) ? wRook : bRook] ^= fromTo;
		bb[move.getPieceColor()] ^= fromTo;
		occupied ^= fromTo;
		break;
	case QUEEN_CASTLING:
		from >>= 4;
		to <<= 1;
		fromTo = from | to;
		bb[(piece < 6) ? wRook : bRook] ^= fromTo;
		bb[move.getPieceColor()] ^= fromTo;
		occupied ^= fromTo;
		break;
	case EN_PASSANT:
		to = 1ULL << ((move.getTo() < 32) ? move.getTo() + 8 : move.getTo() - 8);
		bb[(move.getPieceColor() == Whites) ? Blacks : Whites] ^= to;
		bb[(move.getPiece() == wPawn) ? bPawn : wPawn] ^= to;
		occupied ^= to;
		break;
	case BISHOP_PROM:
		break;
	case KNIGHT_PROM:
		break;
	case ROOK_PROM:
		break;
	case QUEEN_PROM:
		break;
	}
	if (move.getSpecialMove() != DOUBLE_PUSH) {
		enPassantSquare = 0;
	}

	changeTurn();
}

void Board::unMakeMove(Move move)
{
	int piece = move.getPiece();
	U64 from = 1ULL << move.getFrom();
	U64 to = 1ULL << move.getTo();
	U64 fromTo = from | to;
	bb[piece] ^= fromTo;
	bb[move.getPieceColor()] ^= fromTo;
	if (move.isCapture()) {
		bb[move.getCapturedPiece()] ^= to;
		bb[move.getCaptureColor()] ^= to;
		occupied ^= from;
	}
	else
		occupied ^= fromTo;

	switch (piece) {
	case wKing:
		setCastlingRight(wKingSide);
		setCastlingRight(wQueenSide);
		break;
	case bKing:
		setCastlingRight(bKingSide);
		setCastlingRight(bQueenSide);
		break;
	case wRook:
		if (move.getFrom() == 56)
			setCastlingRight(wQueenSide);
		else if (move.getFrom() == 63)
			setCastlingRight(wKingSide);
		break;
	case bRook:
		if (move.getFrom() == 0)
			setCastlingRight(bQueenSide);
		else if (move.getFrom() == 7)
			setCastlingRight(bKingSide);
		break;
	}

	switch (move.getSpecialMove()) {
	case DOUBLE_PUSH:
		enPassantSquare = move.getTo();
		break;
	case KING_CASTLING:
		from <<= 3;
		to >>= 1;
		fromTo = from | to;
		bb[(piece < 6) ? wRook : bRook] ^= fromTo;
		bb[move.getPieceColor()] ^= fromTo;
		occupied ^= fromTo;
		break;
	case QUEEN_CASTLING:
		from >>= 4;
		to <<= 1;
		fromTo = from | to;
		bb[(piece < 6) ? wRook : bRook] ^= fromTo;
		bb[move.getPieceColor()] ^= fromTo;
		occupied ^= fromTo;
		break;
	case EN_PASSANT:
		to = 1ULL << ((move.getTo() < 32) ? move.getTo() + 8 : move.getTo() - 8);
		bb[(move.getPieceColor() == Whites) ? Blacks : Whites] ^= to;
		bb[(move.getPiece() == wPawn) ? bPawn : wPawn] ^= to;
		occupied ^= to;
		break;
	case BISHOP_PROM:
		break;
	case KNIGHT_PROM:
		break;
	case ROOK_PROM:
		break;
	case QUEEN_PROM:
		break;
	}
	if (move.getSpecialMove() != DOUBLE_PUSH) {
		enPassantSquare = 0;
	}
	changeTurn();
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

int Board::getEnPassantSquare() { return enPassantSquare; }
int Board::getCastlingRight(CastlingRights right) { return castlingRights & right; }


void Board::setWhiteTurn(bool turn) { whiteTurn = turn; }
void Board::changeTurn() { whiteTurn = !whiteTurn; }
void Board::setCastlingRight(CastlingRights right) { castlingRights ^= right; }


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
	for (char c: std::string(FEN.begin() + i, FEN.end())) {
		if (c == 'w') {
			setWhiteTurn(true);
		}
		else if (c == 'b') {
			setWhiteTurn(false);
		}
		else if (c == 'k') {
			setCastlingRight(bKingSide);
		}
		else if (c == 'K') {
			setCastlingRight(wKingSide);
		}
		else if (c == 'Q') {
			setCastlingRight(wQueenSide);
		}
		else if (c == 'q') {
			setCastlingRight(bQueenSide);
		}
		else if (isalpha(c) && isdigit(++c)) {
			c--;
			enPassantSquare = c - 97;
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
