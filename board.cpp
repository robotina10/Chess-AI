#pragma once
#include "board.h" 
#include "pawnMoves.h"
#include "knightMoves.h"
#include "kingMoves.h"
#include "slidingPieceMoves.h"

Board Board::copy()
{
	return *this;
}

void Board::init()
{
	setBoard();
	initAttackArrs();
	initTables();
	fillZobristArrs();
}

U64 northOne(U64 b) { return b >> 8; }
U64 southOne(U64 b) { return b << 8; }
U64 eastOne(U64 b) { return (b & notHFile) << 1; }
U64 noEaOne(U64 b) { return (b & notHFile) >> 7; }
U64 soEaOne(U64 b) { return (b & notHFile) << 9; }
U64 westOne(U64 b) { return (b & notAFile) >> 1; }
U64 soWeOne(U64 b) { return (b & notAFile) << 7; }
U64 noWeOne(U64 b) { return (b & notAFile) >> 9; }

U64 inBetweenArr[64][64];

U64 inBetween(int from, int to) {
	return inBetweenArr[from][to];
}

U64 calcInBetween(int sq1, int sq2) {
	const U64 m1 = -1;
	const U64 a2a7 = 0x0001010101010100ULL;
	const U64 b2g7 = 0x0040201008040200ULL;
	const U64 h1b7 = 0x0002040810204080ULL;
	U64 btwn, line, rank, file;

	btwn = (m1 << sq1) ^ (m1 << sq2);
	file = (sq2 & 7) - (sq1 & 7);
	rank = ((sq2 | 7) - sq1) >> 3;
	line = ((file & 7) - 1) & a2a7; /* a2a7 if same file */
	line += 2 * (((rank & 7) - 1) >> 58); /* b1g1 if same rank */
	line += (((rank - file) & 15) - 1) & b2g7; /* b2g7 if same diagonal */
	line += (((rank + file) & 15) - 1) & h1b7; /* h1b7 if same antidiag */
	line *= btwn & -1*btwn; /* mul acts like shift by smaller square */
	return line & btwn;   /* return the bits on that line in-between */
}

void initInBetween()
{
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			inBetweenArr[i][j] = calcInBetween(i, j);
		}
	}
}

int bitScanForward(U64 bb) { //try how much inline would increase performance
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
	if (knightAttacks[to] & bb[wKnight - side]) return true;
	if (pawnAttacks[0 + side][to] & bb[wPawn - side]) return true;
	U64 queenRook = bb[wQueen - side] | bb[wRook - side];
	if (rookAttack(to, occupied) & queenRook) return true;
	U64 queenBishop = bb[wBishop - side] | bb[wQueen - side];
	if (bishopAttack(to, occupied) & queenBishop) return true;
	return false;
}

bool Board::isCheck()
{
	int king = bKing + whiteTurn;
	occupied ^= bb[king];
	bool check = attacked(bitScanForward(bb[king]), whiteTurn);
	occupied ^= bb[king];
	return check;
}

bool Board::isCheckmate(int moveListCount)
{
	return !moveListCount && isCheck();
}

bool Board::isStalemate(int moveListCount)
{
	return !moveListCount && !isCheck();
}

bool Board::isDraw(int moveListCount) // not sure about fifty move rule if it can be evalueted immediatly as draw, there is also some fide rule
{
	return fiftyMoveRule() || threeFoldRepetitionRule() || isStalemate(moveListCount) || isDrawByMaterial();
}

bool Board::isDrawByMaterial()
{
	return false;
}

bool Board::fiftyMoveRule()
{
	return (halfMoveClock >= 100) ? true : false;
}

bool Board::threeFoldRepetitionRule()
{
	int count = 0;
	U64 currentHash = ZobristKey();

	for (int i = history.size() - 2; i >= 0; i -= 2) {
		if (history[i] == currentHash) {
			count++;
			if (count >= 2) return true;
		}
	}
	return false;
}

U64 Board::checkedSquares(bool side)
{
	int king = bKing + side;
	occupied ^= bb[king];
	
	U64 checked = 0;
	U64 pieces = bb[wKnight - side];
	while (pieces) {
		int from = bitScanForwardWithReset(pieces);
		checked |= knightAttacks[from];
	}
	pieces = bb[wRook - side] | bb[wQueen - side];
	while (pieces) {
		int from = bitScanForwardWithReset(pieces);
		checked |= rookAttack(from, occupied);
	}
	pieces = bb[wBishop - side] | bb[wQueen - side];
	while (pieces) {
		int from = bitScanForwardWithReset(pieces);
		checked |= bishopAttack(from, occupied);
	}
	checked |= kingAttacks[bitScanForward(bb[wKing - side])];
	if (side) {
		checked |= soWeOne(bb[bPawn]);
		checked |= soEaOne(bb[bPawn]);
	}
	else {
		checked |= noWeOne(bb[wPawn]);
		checked |= noEaOne(bb[wPawn]);
	}

	occupied ^= bb[king];
	return checked;
}

void Board::findCheckingPieces(CheckingPieces &cp, bool side)
{
	int king = bitScanForward(bb[bKing + side]);
	U64 rookAtt = rookAttack(king, occupied);
	U64 rook = rookAtt & bb[wRook - side];
	if (rook) {
		cp.piece = (Piece)(wRook - side);
		cp.bb |= rook;
	}
	U64 bishopAtt = bishopAttack(king, occupied);
	U64 bishop = bishopAtt & bb[wBishop - side];
	if (bishop) {
		cp.piece = (Piece)(wBishop - side);
		cp.bb |= bishop;
	}
	U64 queen = (rookAtt | bishopAtt) & bb[wQueen - side];
	if (queen) {
		cp.piece = (Piece)(wQueen - side);
		cp.bb |= queen;
	}
	U64 knight = knightAttacks[king] & bb[wKnight - side];
	if (knight) {
		cp.piece = (Piece)(wKnight - side);
		cp.bb |= knight;
	}
	U64 pawn = pawnAttacks[side][king] & bb[wPawn - side];
	if (pawn) {
		cp.piece = (Piece)(wPawn - side);
		cp.bb |= pawn;
	}
	U64 pieces = cp.bb;
	while (pieces) {
		bitScanForwardWithReset(pieces);
		cp.count++;
	}
}

U64 Board::xrayRookAttacks(U64 occ, U64 blockers, int rookSq) {
	U64 attacks = rookAttack(rookSq, occ);
	blockers &= attacks;
	return attacks ^ rookAttack(rookSq, occ ^ blockers);
}

U64 Board::xrayBishopAttacks(U64 occ, U64 blockers, int bishopSq) {
	U64 attacks = bishopAttack(bishopSq, occ);
	blockers &= attacks;
	return attacks ^ bishopAttack(bishopSq, occ ^ blockers);
}

void Board::findPinnedPieces(PinnedPieces &pinnedPieces, bool side)
{
	U64 pinned;
	int ownPieces = Blacks + side;
	int squareOfKing = bitScanForward(bb[bKing + side]);
	U64 pinner = xrayRookAttacks(occupied, bb[ownPieces], squareOfKing) & (bb[wRook - side] | bb[wQueen - side]);
	while (pinner) {
		int sq = bitScanForwardWithReset(pinner);
		pinned = inBetween(sq, squareOfKing) & bb[ownPieces];
		pinnedPieces.all |= pinned;
		if (pinned & (bb[bRook + side] | bb[bQueen + side] | bb[bPawn + side])) {
			if (sq % 8 == squareOfKing % 8)
				pinnedPieces.map[pinned] |= fileAttacks(occupied, bitScanForward(pinned)) & (getEmpty() | bb[Whites - side]);
			else
				pinnedPieces.map[pinned] |= lineAttacks(occupied, bitScanForward(pinned)) & (getEmpty() | bb[Whites - side]);
		}
		else
			pinnedPieces.map[pinned] = 0;
	}
	pinner = xrayBishopAttacks(occupied, bb[ownPieces], squareOfKing) & (bb[wBishop - side] | bb[wQueen - side]);
	while (pinner) {
		int sq = bitScanForwardWithReset(pinner);
		pinned = inBetween(sq, squareOfKing) & bb[ownPieces];
		pinnedPieces.all |= pinned;
		if (pinned & (bb[bBishop + side] | bb[bQueen + side] | bb[bPawn + side])) {
			if (sq % 9 == squareOfKing % 9)
				pinnedPieces.map[pinned] |= diagonalAttacks(occupied, bitScanForward(pinned)) & (getEmpty() | bb[Whites - side]);
			else
				pinnedPieces.map[pinned] |= antiDiagAttacks(occupied, bitScanForward(pinned)) & (getEmpty() | bb[Whites - side]);
		}
		else 
			pinnedPieces.map[pinned] = 0;
	}
}

U64 Board::getPieceAttackingKingDirectAttack(Piece piece, U64 pieceBB)
{
	int from = bitScanForward(pieceBB);
	U64 attack;
	int king = bitScanForward(bb[bKing + whiteTurn]);
	switch (piece + whiteTurn) {
	case wRook:
		attack = fileAttacks(occupied, king);
		if (attack & pieceBB)
			return attack & (fileAttacks(occupied, from) | pieceBB);
		return lineAttacks(occupied, king) & (lineAttacks(occupied, from) | pieceBB);
	case wBishop:
		attack = diagonalAttacks(occupied, king);
		if (attack & pieceBB)
			return attack & (diagonalAttacks(occupied, from) | pieceBB);
		return antiDiagAttacks(occupied, king) & (antiDiagAttacks(occupied, from) | pieceBB);
	case wQueen:
		attack = fileAttacks(occupied, king);
		if (attack & pieceBB)
			return attack & (fileAttacks(occupied, from) | pieceBB);
		attack = lineAttacks(occupied, king);
		if (attack & pieceBB)
			return attack & (lineAttacks(occupied, from) | pieceBB);
		attack = diagonalAttacks(occupied, king);
		if (attack & pieceBB)
			return attack & (diagonalAttacks(occupied, from) | pieceBB);
		attack = antiDiagAttacks(occupied, king);
		if (attack & pieceBB)
			return attack & (antiDiagAttacks(occupied, from) | pieceBB);
	}
}

void Board::generateWhiteMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly)
{
	getQueenMoves(wQueen, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getWhitePawnMoves(moveList, checkingPieces, pinnedPieces, capturesOnly);
	getRookMoves(wRook, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getKnightMoves(wKnight, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getBishopMoves(wBishop, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getKingMoves(wKing, moveList, capturesOnly);
}

void Board::generateBlackMoves(MoveList& moveList, U64 checkingPieces, PinnedPieces pinnedPieces, bool capturesOnly)
{
	getQueenMoves(bQueen, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getBlackPawnMoves(moveList, checkingPieces, pinnedPieces, capturesOnly);
	getRookMoves(bRook, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getKnightMoves(bKnight, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getBishopMoves(bBishop, moveList, checkingPieces, pinnedPieces, capturesOnly);
	getKingMoves(bKing, moveList, capturesOnly);
}

int Board::generateLegalMoves(MoveList& moveList, bool capturesOnly)
{
	CheckingPieces checkingPieces;
	findCheckingPieces(checkingPieces, whiteTurn);
	PinnedPieces pinnedPieces;
	findPinnedPieces(pinnedPieces, whiteTurn);

	if (!checkingPieces.count) {
		checkingPieces.bb = ~0;
		if (whiteTurn)
			generateWhiteMoves(moveList, checkingPieces.bb, pinnedPieces, capturesOnly);
		else
			generateBlackMoves(moveList, checkingPieces.bb, pinnedPieces, capturesOnly);
	}
	else if (checkingPieces.count == 1) {
		if (whiteTurn) {
			if (checkingPieces.piece == bKnight || checkingPieces.piece == bPawn) {
				generateWhiteMoves(moveList, checkingPieces.bb, pinnedPieces, capturesOnly);
			}
			else {
				checkingPieces.bb |= getPieceAttackingKingDirectAttack(checkingPieces.piece, checkingPieces.bb);
				generateWhiteMoves(moveList, checkingPieces.bb, pinnedPieces, capturesOnly);
			}
		}
		else {
			if (checkingPieces.piece == wKnight || checkingPieces.piece == wPawn) {
				generateBlackMoves(moveList, checkingPieces.bb, pinnedPieces, capturesOnly);
			}
			else {
				checkingPieces.bb |= getPieceAttackingKingDirectAttack(checkingPieces.piece, checkingPieces.bb);
				generateBlackMoves(moveList, checkingPieces.bb, pinnedPieces, capturesOnly);
			}
		}
	}
	else {
		getKingMoves((Piece)(bKing + whiteTurn), moveList, capturesOnly);
	}
	return moveList.count;
}

void Board::removeRooksCastlingRights(int rook, int pos)
{
	if (rook == wRook) {
		if (pos == 56)
			removeCastlingRight(wQueenSide);
		else if (pos == 63)
			removeCastlingRight(wKingSide);
	}
	else if (rook == bRook) {
		if (pos == 0)
			removeCastlingRight(bQueenSide);
		else if (pos == 7)
			removeCastlingRight(bKingSide);
	}
}

void Board::makeMove(Move move)
{
	Piece piece = (Piece)move.getPiece();
	Piece capture = (Piece)move.getCapturedPiece();
	int pieceColor = move.getPieceColor();
	int pieceGroup = move.getPieceGroup();
	int captureGroup = Whites - pieceColor;
	U64 from = 1ULL << move.getFrom();
	U64 to = 1ULL << move.getTo();
	U64 fromTo = from | to;
	bb[piece] ^= fromTo;
	bb[pieceGroup] ^= fromTo;
	if (move.isCapture()) {
		bb[capture] ^= to;
		bb[captureGroup] ^= to;
		occupied ^= from;
		piecesCount[capture]--;
		halfMoveClock = 0;
		removeRooksCastlingRights(capture, move.getTo());
	}
	else {
		occupied ^= fromTo;
	}

	removeRooksCastlingRights(piece, move.getFrom());
	if (piece == wKing) {
		removeCastlingRight(wKingSide);
		removeCastlingRight(wQueenSide);
	}
	else if (piece == bKing) {
		removeCastlingRight(bKingSide);
		removeCastlingRight(bQueenSide);
	}
 
	switch (move.getSpecialMove()) {
	case DOUBLE_PUSH:
		enPassantSquare = move.getTo();
		break;
	case KING_CASTLING:
		from <<= 3;
		to >>= 1;
		fromTo = from | to;
		bb[bRook + pieceColor] ^= fromTo;
		bb[pieceGroup] ^= fromTo;
		occupied ^= fromTo;
		break;
	case QUEEN_CASTLING:
		from >>= 4;
		to <<= 1;
		fromTo = from | to;
		bb[bRook + pieceColor] ^= fromTo;
		bb[pieceGroup] ^= fromTo;
		occupied ^= fromTo;
		break;
	case EN_PASSANT:
		to = 1ULL << ((move.getTo() < 32) ? move.getTo() + 8 : move.getTo() - 8);
		bb[captureGroup] ^= to;
		bb[wPawn - pieceColor] ^= to;
		occupied ^= to;
		piecesCount[wPawn - pieceColor]--;
		break;
	case BISHOP_PROM:
		bb[piece] ^= to;
		bb[bBishop + pieceColor] ^= to;
		piecesCount[bBishop + pieceColor]++;
		break;
	case KNIGHT_PROM:
		bb[piece] ^= to;
		bb[bKnight + pieceColor] ^= to;
		piecesCount[bKnight + pieceColor]++;
		break;
	case ROOK_PROM:
		bb[piece] ^= to;
		bb[bRook + pieceColor] ^= to;
		piecesCount[bRook + pieceColor]++;
		break;
	case QUEEN_PROM:
		bb[piece] ^= to;
		bb[bQueen + pieceColor] ^= to;
		piecesCount[bQueen + pieceColor]++;
		break;
	}
	if (move.getSpecialMove() != DOUBLE_PUSH) {
		enPassantSquare = 0;
	}
	if (piece == wPawn || piece == bPawn || capture == wPawn || capture == bPawn)
		halfMoveClock = 0;
	else
		halfMoveClock++;
	if (!whiteTurn)
		fullMoveCounter++;
	history.push_back(ZobristKey());
	changeTurn();
}

void Board::unMakeMove(Move move, PosInfo posInfo)
{
	halfMoveClock = posInfo.getHalfMoveClock();
	castlingRights = posInfo.getCastlingRights();
	enPassantSquare = posInfo.getEpSquare();
	Piece piece = (Piece)move.getPiece();
	int pieceColor = move.getPieceColor();
	int pieceGroup = move.getPieceGroup();
	int captureGroup = Whites - pieceColor;
	U64 from = 1ULL << move.getFrom();
	U64 to = 1ULL << move.getTo();
	U64 fromTo = from | to;
	bb[piece] ^= fromTo;
	bb[pieceGroup] ^= fromTo;
	if (move.isCapture()) {
		int capture = move.getCapturedPiece();
		bb[capture] ^= to;
		bb[captureGroup] ^= to;
		occupied ^= from;
		piecesCount[capture]++;
	}
	else {
		occupied ^= fromTo;
	}
	switch (move.getSpecialMove()) {;
	case KING_CASTLING:
		from <<= 1;
		to <<= 1;
		fromTo = from | to;
		bb[bRook + pieceColor] ^= fromTo;
		bb[pieceGroup] ^= fromTo;
		occupied ^= fromTo;
		break;
	case QUEEN_CASTLING:
		from >>= 4;
		to <<= 1;
		fromTo = from | to;
		bb[bRook + pieceColor] ^= fromTo;
		bb[pieceGroup] ^= fromTo;
		occupied ^= fromTo;
		break;
	case EN_PASSANT:
		to = 1ULL << ((move.getTo() < 32) ? move.getTo() + 8 : move.getTo() - 8);
		bb[captureGroup] ^= to;
		bb[wPawn - pieceColor] ^= to;
		occupied ^= to;
		piecesCount[wPawn - pieceColor]++;
		break;
	case BISHOP_PROM:
		bb[piece] ^= to;
		bb[bBishop + pieceColor] ^= to;
		piecesCount[bBishop + pieceColor]--;
		break;
	case KNIGHT_PROM:
		bb[piece] ^= to;
		bb[bKnight + pieceColor] ^= to;
		piecesCount[bKnight + pieceColor]--;
		break;
	case ROOK_PROM:
		bb[piece] ^= to;
		bb[bRook + pieceColor] ^= to;
		piecesCount[bRook + pieceColor]--;
		break;
	case QUEEN_PROM:
		bb[piece] ^= to;
		bb[bQueen + pieceColor] ^= to;
		piecesCount[bQueen + pieceColor]--;
		break;
	}
	if (!whiteTurn)
		fullMoveCounter--;
	history.pop_back();
	changeTurn();
}

void Board::initAttackArrs()
{
	initPawnAttacks();
	initKnightAttacks();
	initKingAttacks();
	initSlidingMasks();
	initInBetween();
}

U64 Board::getEmpty() { return ~occupied; };
U64 Board::getBitboard(int index) { return bb[index]; }
U64 Board::getBitboard(Piece index) { return bb[index]; }
U64 Board::getEnemy(Piece piece) { return bb[pieceColor::getOpponentGroup(piece)]; }
bool Board::isWhiteTurn() { return whiteTurn; }
bool Board::isEmpty(int pos) { return getEmpty() & (1ULL << pos); }

int Board::getCastlingRight(CastlingRights right) { return castlingRights & right; }

PosInfo Board::getPosInfo() { return PosInfo(castlingRights, enPassantSquare, halfMoveClock); }

void Board::setWhiteTurn(bool turn) { whiteTurn = turn; }
void Board::changeTurn() { whiteTurn = whiteTurn ^ 1; }
void Board::removeCastlingRight(CastlingRights right) { castlingRights = castlingRights & ~right; }
void Board::setCastlingRight(CastlingRights right) { castlingRights |= right; }


Piece Board::getPiece(int pos)
{
	if (!((1ULL << pos) && occupied))
		return EMPTY;

	if ((bb[Whites] & (1ULL << pos)) >> pos) {
		for (int i = 1; i < 12; i+=2) {
			if ((bb[i] & (1ULL << pos)))
				return (Piece)i;
		}
	}
	if ((bb[Blacks] & (1ULL << pos)) >> pos) {
		for (int i = 0; i < 10; i+=2) {
			if ((bb[i] & (1ULL << pos)))
				return (Piece)i;
		}
	}
}

void Board::placePiece(Piece piece, int pos)
{
	bb[piece] |= 1ULL << pos;
	bb[Blacks + pieceColor::getPieceColor(piece)] |= 1ULL << pos;
	occupied ^= 1ULL << pos;
	piecesCount[piece]++;
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
			file += c - '0';
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
	const char* ptr = FEN.c_str();
	ptr += i;
	while (*ptr) {
		char c = *ptr;
		int spaces = 0;
		if (c == ' ')
			spaces++;
		else if (c == 'w') {
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
		else if (isalpha(c) && isdigit(*(++ptr))) {
			enPassantSquare = (8 - (*ptr - '1')) * 8;
			enPassantSquare += c - 'a';
		}
		else if (spaces > 2) {
			std::string temp;
			for (char ch : std::string(FEN.begin() + (FEN.c_str() - ptr), FEN.end())) {
				if (ch >= '0' && ch <= '9') {
					temp += ch;  // Add digit to temp string
				}
				else {
					if (!temp.empty()) {
						halfMoveClock = std::stoi(temp);
						temp.clear();  // Clear temp for the next number
					}
				}
			}
			fullMoveCounter = std::stoi(temp);
			break;
		}
		ptr++;
	}
}

std::string Board::saveBoardToFen()
{
	const char files[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
	std::string fen;
	int file = 0;
	int rank = 0;
	int spaceBetweenPieces = 0;

	for (int i = 0; i < 64; i++) {
		file++;
		if ((1ULL << i) & occupied) {
			if (spaceBetweenPieces) {
				if (fen.size()) {
					if (fen.back() == '/' || isalpha(fen.back()))
						fen.push_back(spaceBetweenPieces + '0');
					else {
						fen.pop_back();
						fen.push_back(spaceBetweenPieces + '0');
					}
				}
				spaceBetweenPieces = 0;
			}
			char piece;
			switch (getPiece(i)) {
			case bKing:
				piece = 'k';
				break;
			case bQueen:
				piece = 'q';
				break;
			case bRook:
				piece = 'r';
				break;
			case bBishop:
				piece = 'b';
				break;
			case bKnight:
				piece = 'n';
				break;
			case bPawn:
				piece = 'p';
				break;
			case wKing:
				piece = 'K';
				break;
			case wQueen:
				piece = 'Q';
				break;
			case wRook:
				piece = 'R';
				break;
			case wBishop:
				piece = 'B';
				break;
			case wKnight:
				piece = 'N';
				break;
			case wPawn:
				piece = 'P';
				break;
			}
			fen.push_back(piece);
		}
		else 
			spaceBetweenPieces++;

		if (file == 8) {
			file = 0;
			if (spaceBetweenPieces > 0)
				fen.push_back(spaceBetweenPieces + '0');
			spaceBetweenPieces = 0;
			rank++;
			if (rank < 8)
				fen.append("/");
		}
	}
	fen.append((whiteTurn) ? " w " : " b ");
	if (castlingRights) {
		if (getCastlingRight(wKingSide))
			fen.append("K");
		if (getCastlingRight(wQueenSide))
			fen.append("Q");
		if (getCastlingRight(bKingSide))
			fen.append("k");
		if (getCastlingRight(bQueenSide))
			fen.append("q");
		fen.push_back(' ');
	}
	else
		fen.append(" - ");

	if (enPassantSquare) {
		fen.push_back(files[enPassantSquare % 8]);
		fen.push_back(enPassantSquare / 8 + '0');
		fen.append(" ");
	}
	else 
		fen.append("- ");
	fen.append(std::to_string(halfMoveClock) + " ");
	fen.append(std::to_string(fullMoveCounter));
	return fen;
}

void Board::printBB(U64 bb)
{
	for (int i = 0; i < 64; i++) {
		((bb >> i) & 1ULL) ? std::cout << 1 << " " : std::cout << 0 << " ";
		if ((i + 1) % 8 == 0) {
			std::cout << "\n";
		}
	}
	std::cout << "\n";
}