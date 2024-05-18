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
	if (knightAttacks[to] & bb[bKnight + side]) return true;
	if (pawnAttacks[0][to] & bb[bPawn + side]) return true;
	U64 queenRook = bb[bQueen + side] | bb[bRook + side];
	if (rookAttack(to, occupied) & queenRook) return true;
	U64 queenBishop = bb[bBishop + side] | bb[bQueen + side];
	if (bishopAttack(to, occupied) & queenBishop) return true;
}

bool Board::inCheck(int to, bool side)
{
	U64 king = bKing + side;
	occupied ^= bb[king];
	bool check = attacked(to, side);
	occupied ^= bb[king];
	return check;
}

U64 Board::getEnemyAttack(bool side)
{
	/*U64 knights = bb[bKnight];

	while (knights) {
		int from = bitScanForwardWithReset(knights);
		U64 attack = knightAttacks[from] & (getEmpty() | getEnemy(knight));
	U64 westCaptures = bPawnsCaptureWest(bb[bPawn], bb[Whites]) & checkingPieces.bb;
	getMovesFromPawnCaptureBB(moveList, westCaptures, bPawn, -7);
	U64 eastCaptures = bPawnsCaptureEast(bb[bPawn], bb[Whites]) & checkingPieces.bb;
	getMovesFromPawnCaptureBB(moveList, eastCaptures, bPawn, -9);
	U64 queenRook = bb[bQueen] | bb[bRook];
	U64 queenBishop = bb[bBishop] | bb[bQueen];
	return knightAttacks[to] & bb[bKnight]
		| pawnAttacks[1][to] & bb[bPawn]
		| rookAttack(to, occupied) & queenRook
		| bishopAttack(to, occupied) & queenBishop;
	*/
	return 0; 
}

void Board::findCheckingPieces(CheckingPieces &cp, bool side)
{
	side = !side;
	int king = bitScanForward(bb[wKing + side]);
	U64 knight = knightAttacks[king] & bb[bKnight + side];
	if (knight) {
		cp.count++;
		cp.piece = (Pieces)(bKnight + side);
		cp.bb = knight;
	}
	U64 pawn = pawnAttacks[1][king] & bb[bPawn + side];
	if (pawn) {
		cp.count++;
		cp.piece = (Pieces)(bPawn + side);
		cp.bb = pawn;
	}
	U64 rookAtt = rookAttack(king, occupied);
	U64 rook = rookAtt & bb[bRook + side];
	if (rook) {
		cp.count++;
		cp.piece = (Pieces)(bRook + side);;
		cp.bb = rook;
	}
	U64 bishopAtt = bishopAttack(king, occupied);
	U64 bishop = bishopAtt & bb[bBishop + side];
	if (bishop) {
		cp.count++;
		cp.piece = (Pieces)(bBishop);
		cp.bb = bishop;
	}
	U64 queen = (rookAtt | bishopAtt) & bb[bQueen + side];
	if (queen) {
		cp.count++;
		cp.piece = (Pieces)(bQueen + side);
		cp.bb = queen;
	}
}

U64 Board::getPieceAttack(Pieces piece, int from)
{
	int king;
	U64 r;
	U64 b;
	king = bitScanForward(bb[bKing + whiteTurn]);
	switch (piece + whiteTurn) {
	case bBishop:
		return bishopAttack(king, occupied) & bishopAttack(from, occupied);
	case bQueen:
		r = rookAttack(from, occupied) & rookAttack(king, occupied);
		b = bishopAttack(from, occupied) & bishopAttack(king, occupied);
		if (r)
			return r;
		else
			return b;
	case bRook:
		return rookAttack(king, occupied) & rookAttack(from, occupied);
	}
}

void Board::generateWhiteMoves(MoveList& moveList, CheckingPieces checkingPieces)
{
	getWhitePawnMoves(moveList, checkingPieces);
	getKnightMoves(wKnight, moveList, checkingPieces);
	getKingMoves(wKing, moveList);
	getRookMoves(wRook, moveList, checkingPieces);
	getBishopMoves(wBishop, moveList, checkingPieces);
	getQueenMoves(wQueen, moveList, checkingPieces);
}

void Board::generateBlackMoves(MoveList& moveList, CheckingPieces checkingPieces)
{
	getBlackPawnMoves(moveList, checkingPieces);
	getKnightMoves(bKnight, moveList, checkingPieces);
	getKingMoves(bKing, moveList);
	getRookMoves(bRook, moveList, checkingPieces);
	getBishopMoves(bBishop, moveList, checkingPieces);
	getQueenMoves(bQueen, moveList, checkingPieces);
}

U64 Board::xrayRookAttacks(U64 occ, U64 blockers, int rookSq) {
	U64 attacks = rookAttack(occ, rookSq);
	blockers &= attacks;
	return attacks ^ rookAttack(occ ^ blockers, rookSq);
}

U64 Board::xrayBishopAttacks(U64 occ, U64 blockers, int bishopSq) {
	U64 attacks = bishopAttack(occ, bishopSq);
	blockers &= attacks;
	return attacks ^ bishopAttack(occ ^ blockers, bishopSq);
}

bool Board::isPinned(int from, bool side)
{
	U64 pinned = 0;
	U64 pinner = xrayRookAttacks(occupied, bb[Whites], bitScanForward(bb[wKing]) & (bb[bRook] | bb[bQueen]);
	while (pinner) {
		int sq = bitScanForward(pinner);
		pinned |= obstructed(sq, squareOfKing) & ownPieces;
		pinner &= pinner - 1;
	}
	pinner = xrayBishopAttacks(occupiedBB, ownPieces, squareOfKing) & opBQ;
	while (pinner) {
		int sq = bitScanForward(pinner);
		pinned |= obstructed(sq, squareOfKing) & ownPieces;
		pinner &= pinner - 1;
	}
}

int Board::generateLegalMoves(MoveList& moveList)
{
	CheckingPieces checkingPieces;
	findCheckingPieces(checkingPieces, whiteTurn);
	if (!checkingPieces.count) {
		checkingPieces.bb = ~0;
		if (whiteTurn)
			generateWhiteMoves(moveList, checkingPieces);
		else
			generateBlackMoves(moveList, checkingPieces);
	}
	else if (checkingPieces.count == 1) {
		if (whiteTurn) {
			if (checkingPieces.piece == bKnight || checkingPieces.piece == bPawn) {
				generateWhiteMoves(moveList, checkingPieces);
			}
			else {
				checkingPieces.bb |= getPieceAttack(checkingPieces.piece, bitScanForward(checkingPieces.bb));
				generateWhiteMoves(moveList, checkingPieces);
			}
		}
		else {
			if (checkingPieces.piece == wKnight || checkingPieces.piece == wPawn) {
				generateBlackMoves(moveList, checkingPieces);
			}
			else {
				checkingPieces.bb |= getPieceAttack(checkingPieces.piece, bitScanForward(checkingPieces.bb));
				generateBlackMoves(moveList, checkingPieces);
			}
		}
	}
	else {
		getKingMoves(bKing + whiteTurn, moveList);
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
		bb[(getPieceColor(piece) == Whites) ? wRook : bRook] ^= fromTo;
		bb[move.getPieceColor()] ^= fromTo;
		occupied ^= fromTo;
		break;
	case QUEEN_CASTLING:
		from >>= 4;
		to <<= 1;
		fromTo = from | to;
		bb[(getPieceColor(piece) == Whites) ? wRook : bRook] ^= fromTo;
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
		bb[(getPieceColor(piece) == Whites) ? wRook : bRook] ^= fromTo;
		bb[move.getPieceColor()] ^= fromTo;
		occupied ^= fromTo;
		break;
	case QUEEN_CASTLING:
		from >>= 4;
		to <<= 1;
		fromTo = from | to;
		bb[(getPieceColor(piece) == Whites) ? wRook : bRook] ^= fromTo;
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
U64 Board::getBitboard(int index) { return bb[index]; }
U64 Board::getBitboard(Pieces index) { return bb[index]; }
U64 Board::getEnemy(Pieces piece) { return bb[getPieceColor(piece)]; }
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
		for (int i = 1; i < 12; i+=2) {
			if ((bb[i] & (1ULL << pos)))
				return (Pieces)i;
		}
	}
	if ((bb[Blacks] & (1ULL << pos)) >> pos) {
		for (int i = 0; i < 11; i+=2) {
			if ((bb[i] & (1ULL << pos)))
				return (Pieces)i;
		}
	}
	return EMPTY;
}

void Board::placePiece(Pieces piece, int pos)
{
	bb[piece] |= 1ULL << pos;
	bb[getPieceColor(piece)] |= 1ULL << pos;
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
