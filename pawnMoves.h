#pragma once
#include "board.h"

const U64 rank8 = 0xff00000000000000;
const U64 rank1 = 0xff;

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

U64 wPawnsEnPassant()
{
	return 0;
}

U64 bPawnsEnPassant()
{
	return 0;
}

bool isPromotion(int to)
{
	return (to > 55 || to < 8) ? true : false;
}

void getMovesFromPushBB(MoveList& moveList, U64 bb, Pieces piece, int pushDistance)
{
	while (bb) {
		int to = bitScanForwardWithReset(bb);
		if (isPromotion(to)) {
			moveList.moves[moveList.count++] = Move(to + pushDistance, to, piece, EMPTY, GENERIC_PROM);
		}
		else
			moveList.moves[moveList.count++] = Move(to + pushDistance, to, piece, EMPTY, NONE);
	}
}

void Board::getMovesFromPawnCaptureBB(MoveList& moveList, U64 bb, Pieces piece, int captureDistance)
{
	while (bb) {
		int to = bitScanForwardWithReset(bb);
		Pieces capturedPiece = getPiece(to);
		if (isPromotion(to)) {
			moveList.moves[moveList.count++] = Move(to + captureDistance, to, piece, capturedPiece, GENERIC_PROM);

		}
		else
			moveList.moves[moveList.count++] = Move(to + captureDistance, to, piece, capturedPiece, NONE);
	}
}

void Board::getWhitePawnMoves(MoveList& moveList)
{
	U64 empty = getEmpty();
	U64 push = wPawnsPush(wPawnsCanPush(bb[wPawn], empty), empty);
	getMovesFromPushBB(moveList, push, wPawn, 8);

	U64 doublePush = wPawnsDoublePush(wPawnsCanDoublePush(bb[wPawn], empty), empty);
	getMovesFromPushBB(moveList, doublePush, wPawn, 16);

	U64 westCaptures = wPawnsCaptureWest(bb[wPawn], bb[Blacks]);
	getMovesFromPawnCaptureBB(moveList, westCaptures, wPawn, 9);
	U64 eastCaptures = wPawnsCaptureEast(bb[wPawn], bb[Blacks]);
	getMovesFromPawnCaptureBB(moveList, eastCaptures, wPawn, 7);
	
	wPawnsEnPassant();
}


void Board::getBlackPawnMoves(MoveList& moveList)
{
	U64 empty = getEmpty();
	U64 push = bPawnsPush(bPawnsCanPush(bb[bPawn], empty), empty);
	getMovesFromPushBB(moveList, push, bPawn, -8);

	U64 doublePush = bPawnsDoublePush(bPawnsCanDoublePush(bb[bPawn], empty), empty);
	getMovesFromPushBB(moveList, doublePush, bPawn, -16);

	U64 westCaptures = bPawnsCaptureWest(bb[bPawn], bb[Whites]);
	getMovesFromPawnCaptureBB(moveList, westCaptures, bPawn, -7);
	U64 eastCaptures = bPawnsCaptureEast(bb[bPawn], bb[Whites]);
	getMovesFromPawnCaptureBB(moveList, eastCaptures, bPawn, -9);

	bPawnsEnPassant();
}
