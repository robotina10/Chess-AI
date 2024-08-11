#pragma once
#include "board.h"

U64 pawnAttacks[2][64];

void initPawnAttacks()
{
	for (int i = 0; i < 64; i++) {
		U64 pawn = 1ULL << i;
		pawnAttacks[1][i] = noWeOne(pawn) | noEaOne(pawn);
		pawnAttacks[0][i] = soWeOne(pawn) | soEaOne(pawn);
	}
}

U64 wPawnsPush(U64 wp, U64 empty)
{
	return northOne(wp) & empty;
}
U64 wPawnsDoublePush(U64 wp, U64 empty)
{
	const U64 rank4 = 0x000000FF00000000;
	U64 push = wPawnsPush(wp, empty);
	return northOne(push) & empty & rank4;
}

U64 wPawnsCanPush(U64 wp, U64 empty)
{
	return wp & southOne(empty);
}
U64 wPawnsCanDoublePush(U64 wp, U64 empty)
{
	const U64 rank4 = 0x000000FF00000000;
	U64 emptyRank3 = southOne(empty & rank4) & empty;
	return wPawnsCanPush(wp, emptyRank3);
}

U64 bPawnsPush(U64 bp, U64 empty)
{
	return southOne(bp) & empty;
}
U64 bPawnsDoublePush(U64 bp, U64 empty)
{
	const U64 rank5 = 0x00000000FF000000;
	U64 push = bPawnsPush(bp, empty);
	return southOne(push) & empty & rank5;
}

U64 bPawnsCanPush(U64 bp, U64 empty)
{
	return bp & northOne(empty);
}
U64 bPawnsCanDoublePush(U64 bp, U64 empty)
{
	const U64 rank5 = 0x00000000FF000000;
	U64 emptyRank6 = northOne(empty & rank5) & empty;
	return bPawnsCanPush(bp, emptyRank6);
}

U64 wPawnsCaptureWest(U64 wp, U64 enemy)
{
	return noWeOne(wp) & enemy;
}

U64 wPawnsCaptureEast(U64 wp, U64 enemy)
{
	return noEaOne(wp) & enemy;
}

U64 bPawnsCaptureWest(U64 bp, U64 enemy)
{
	return soWeOne(bp) & enemy;
}

U64 bPawnsCaptureEast(U64 bp, U64 enemy)
{
	return soEaOne(bp) & enemy;
}

U64 wPawnsEnPassant(int enPassantSquare, U64 wp)
{
	if ((1ULL << enPassantSquare) & 0x00000000ff000000) {
		return pawnAttacks[1][enPassantSquare + 8] & wp;
	}
	return 0;
}

U64 bPawnsEnPassant(int enPassantSquare, U64 bp)
{
	if ((1ULL << enPassantSquare) & 0x000000ff00000000) {
		return pawnAttacks[0][enPassantSquare - 8] & bp;
	}
	return 0;
}

bool isPromotion(int to)
{
	return (to > 55 || to < 8) ? true : false;
}

void Board::getEnPassantMoves(MoveList& moveList, U64 bb, Pieces piece, int dir)
{
	while (bb) {
		int from = bitScanForwardWithReset(bb);
		Move move(from, enPassantSquare + dir, piece, EMPTY, EN_PASSANT);
		moveList.moves[moveList.count++] = move;
	}
}

void Board::getMovesFromPushBB(MoveList& moveList, U64 bb, Pieces piece, int pushDistance, SpecialMove doublePush)
{
	while (bb) {
		int to = bitScanForwardWithReset(bb);
		Move move;
		if (isPromotion(to)) {
			move = { to + pushDistance, to, piece, EMPTY, QUEEN_PROM };
			moveList.moves[moveList.count++] = move;
			move = { to + pushDistance, to, piece, EMPTY, KNIGHT_PROM };
			moveList.moves[moveList.count++] = move;
			move = { to + pushDistance, to, piece, EMPTY, ROOK_PROM };
			moveList.moves[moveList.count++] = move;
			move = { to + pushDistance, to, piece, EMPTY, BISHOP_PROM };
			moveList.moves[moveList.count++] = move;
		}
		else {
			move = { to + pushDistance, to, piece, EMPTY, doublePush };
			moveList.moves[moveList.count++] = move;
		}
	}
}

void Board::getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Pieces piece, int captureDistance)
{
	while (bb) {
		int to = bitScanForwardWithReset(bb);
		Move move;
		Pieces capturedPiece = getPiece(to);
		if (isPromotion(to)) {
			move = { to + captureDistance, to, piece, capturedPiece, QUEEN_PROM };
			moveList.moves[moveList.count++] = move;
			move = { to + captureDistance, to, piece, capturedPiece, KNIGHT_PROM };
			moveList.moves[moveList.count++] = move;
			move = { to + captureDistance, to, piece, capturedPiece, ROOK_PROM };
			moveList.moves[moveList.count++] = move;
			move = { to + captureDistance, to, piece, capturedPiece, BISHOP_PROM };
			moveList.moves[moveList.count++] = move;
		}
		else {
			move = { to + captureDistance, to, piece, capturedPiece, NONE };
			moveList.moves[moveList.count++] = move;
		}
	}
}

void Board::getWhitePawnMoves(MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces)
{
	U64 pawns = bb[wPawn] ^ (pinnedPieces.all & bb[wPawn]);
	U64 empty = getEmpty();
	for (int i = 0; i < 2; i++) {
		U64 push = wPawnsPush(wPawnsCanPush(pawns, empty), empty) & checkingPieces.bb;
		getMovesFromPushBB(moveList, push, wPawn, 8, NONE);

		U64 doublePush = wPawnsDoublePush(wPawnsCanDoublePush(pawns, empty), empty) & checkingPieces.bb;
		getMovesFromPushBB(moveList, doublePush, wPawn, 16, DOUBLE_PUSH);

		U64 westCaptures = wPawnsCaptureWest(pawns, bb[Blacks]) & checkingPieces.bb;
		getMovesFromPawnCaptureBB(moveList, westCaptures, wPawn, 9);
		U64 eastCaptures = wPawnsCaptureEast(pawns, bb[Blacks]) & checkingPieces.bb;
		getMovesFromPawnCaptureBB(moveList, eastCaptures, wPawn, 7);

		PinnedPieces e;
		U64 ep = 1ULL << enPassantSquare;
		bb[bPawn] ^= ep;
		occupied ^= ep;
		findPinnedPieces(e, whiteTurn);
		bb[bPawn] ^= ep;
		occupied ^= ep;
		if (!(e.all & pawns)) {
			U64 enPassant = wPawnsEnPassant(enPassantSquare, pawns) & checkingPieces.bb;
			getEnPassantMoves(moveList, enPassant, wPawn, -8);
		}
		pawns = pinnedPieces.all & bb[wPawn];
		if (!pawns)
			return;
		while (pawns) {
			checkingPieces.bb &= pinnedPieces.map[bitScanForwardWithReset(pawns)];
		}
		pawns = pinnedPieces.all & bb[bPawn];
	}
}


void Board::getBlackPawnMoves(MoveList& moveList, CheckingPieces checkingPieces, PinnedPieces pinnedPieces)
{
	U64 pawns = bb[bPawn] ^ (pinnedPieces.all & bb[bPawn]);
	U64 empty = getEmpty();
	for (int i = 0; i < 2; i++) {
		U64 push = bPawnsPush(bPawnsCanPush(pawns, empty), empty) & checkingPieces.bb;
		getMovesFromPushBB(moveList, push, bPawn, -8, NONE);

		U64 doublePush = bPawnsDoublePush(bPawnsCanDoublePush(pawns, empty), empty) & checkingPieces.bb;
		getMovesFromPushBB(moveList, doublePush, bPawn, -16, DOUBLE_PUSH);

		U64 westCaptures = bPawnsCaptureWest(pawns, bb[Whites]) & checkingPieces.bb;
		getMovesFromPawnCaptureBB(moveList, westCaptures, bPawn, -7);
		U64 eastCaptures = bPawnsCaptureEast(pawns, bb[Whites]) & checkingPieces.bb;
		getMovesFromPawnCaptureBB(moveList, eastCaptures, bPawn, -9);

		PinnedPieces e;
		U64 ep = 1ULL << enPassantSquare;
		bb[wPawn] ^= ep;
		occupied ^= ep;
		findPinnedPieces(e, whiteTurn);
		bb[wPawn] ^= ep;
		occupied ^= ep;
		if (!(e.all & pawns)) {
			U64 enPassant = bPawnsEnPassant(enPassantSquare, pawns) & checkingPieces.bb;
			getEnPassantMoves(moveList, enPassant, bPawn, 8);
		}
		pawns = pinnedPieces.all & bb[bPawn];
		if (!pawns)
			return;
		while (pawns) {
			checkingPieces.bb &= pinnedPieces.map[bitScanForwardWithReset(pawns)];
		}
		pawns = pinnedPieces.all & bb[bPawn];

	}
}
