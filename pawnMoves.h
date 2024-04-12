#pragma once
#include "board.h"

/*U64 wPawnAttacks[64];
U64 bPawnAttacks[64];

void preCalculatePawnAttacks()
{
	for (int i = 0; i < 64; i++) {
		U64 pawn = 1ULL << i;
		wPawnAttacks[i] = noWeOne(pawn) | noEaOne(pawn);
		bPawnAttacks[i] = soWeOne(pawn) | soEaOne(pawn);
	}
}*/

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

U64 wPawnsPromotion()
{
	return 0;
}


U64 bPawnsPromotion()
{
	return 0;
}

U64 wPawnsEnPassant()
{
	return 0;
}

U64 bPawnsEnPassant()
{
	return 0;
}

void getMovesFromPushBB(MoveList& moveList, U64 bb, Pieces piece, int doublePush, int pushDistance)
{
	while (bb) {
		int to = bitScanForwardWithReset(bb);
		moveList.moves[moveList.count++] = Move(to + pushDistance, to, piece, EMPTY, 0, doublePush);
	}
}

void Board::getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Pieces piece, int captureDistance)
{
	while (bb) {
		int to = bitScanForwardWithReset(bb);
		Pieces capturedPiece = getPiece(to);
		moveList.moves[moveList.count++] = Move(to + captureDistance, to, piece, capturedPiece, 0, 0);
	}
}

void Board::getWhitePawnMoves(MoveList& moveList)
{
	U64 empty = getEmpty();
	U64 push = wPawnsPush(wPawnsCanPush(bb[wPawn], empty), empty);
	getMovesFromPushBB(moveList, push, wPawn, 0, 8);

	U64 doublePush = wPawnsDoublePush(wPawnsCanDoublePush(bb[wPawn], empty), empty);
	getMovesFromPushBB(moveList, doublePush, wPawn, DOUBLE_PUSH, 16);

	U64 westCaptures = wPawnsCaptureWest(bb[wPawn], bb[Blacks]);
	getMovesFromPawnCaptureBB(moveList, westCaptures, wPawn, 9);
	U64 eastCaptures = wPawnsCaptureEast(bb[wPawn], bb[Blacks]);
	getMovesFromPawnCaptureBB(moveList, eastCaptures, wPawn, 7);
	
	wPawnsPromotion();
	wPawnsEnPassant();
}


void Board::getBlackPawnMoves(MoveList& moveList)
{
	U64 empty = getEmpty();
	U64 push = bPawnsPush(bPawnsCanPush(bb[bPawn], empty), empty);
	getMovesFromPushBB(moveList, push, bPawn, 0, -8);

	U64 doublePush = bPawnsDoublePush(bPawnsCanDoublePush(bb[bPawn], empty), empty);
	getMovesFromPushBB(moveList, doublePush, bPawn, DOUBLE_PUSH, -16);

	U64 westCaptures = bPawnsCaptureWest(bb[bPawn], bb[Whites]);
	getMovesFromPawnCaptureBB(moveList, westCaptures, bPawn, -7);
	U64 eastCaptures = bPawnsCaptureEast(bb[bPawn], bb[Whites]);
	getMovesFromPawnCaptureBB(moveList, eastCaptures, bPawn, -9);

	bPawnsPromotion();
	bPawnsEnPassant();
}
