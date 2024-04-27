#pragma once
#include "board.h"

/* HYPERBOLA QUINTESSSENCE */

U64 reverse(U64 bb) {
	const U64 k1 = 0x5555555555555555;
	const U64 k2 = 0x3333333333333333;
	const U64 k4 = 0x0f0f0f0f0f0f0f0f;
	bb = ((bb >> 1) & k1) | ((bb & k1) << 1);
	bb = ((bb >> 2) & k2) | ((bb & k2) << 2);
	bb = ((bb >> 4) & k4) | ((bb & k4) << 4);
	return bb;
}

U64 singleBB[64];
U64 rankMask[64];
U64 fileMask[64];
U64 diagonalMask[64];
U64 antiDiagMask[64];

void initSlidingMasks()
{
	const U64 diagonal = 0x8040201008040201;
	const U64 antiDiagonal = 0x0102040810204080;
	for (int i = 0; i < 64; i++) {
		U64 piece = 1ULL << i;
		singleBB[i] = piece;
		rankMask[i] = (0xffULL << (i / 8 * 8)) ^ piece;
		fileMask[i] = (0x0101010101010101 << (i & 7)) ^ piece;
		int diag = (i & 7) - (i >> 3);
		diagonalMask[i] = (diag >= 0 ? diagonal >> diag * 8 : diagonal << -diag * 8) ^ piece;
		diag = 7 - (i & 7) - (i >> 3);
		antiDiagMask[i] = (diag >= 0 ? antiDiagonal >> diag * 8 : antiDiagonal << -diag * 8) ^ piece;
	}
}

U64 lineAttacks(U64 occ, int pos)
{
	U64 left = (occ & rankMask[pos]) - 2 * singleBB[pos];
	U64 right = reverse(reverse(occ & rankMask[pos]) - 2 * reverse(singleBB[pos]));
	return (left ^ right) &rankMask[pos];
}

U64 fileAttacks(U64 occ, int pos)
{
	U64 forward = occ & fileMask[pos];
	U64 reverse = _byteswap_uint64(forward);
	forward -= singleBB[pos];
	reverse -= _byteswap_uint64(singleBB[pos]);
	forward ^= _byteswap_uint64(reverse);
	return forward &= fileMask[pos];
}

U64 diagonalAttacks(U64 occ, int pos)
{
	U64 forward = occ & diagonalMask[pos];
	U64 reverse = _byteswap_uint64(forward);
	forward -= singleBB[pos];
	reverse -= _byteswap_uint64(singleBB[pos]);
	forward ^= _byteswap_uint64(reverse);
	return forward &= diagonalMask[pos];
}

U64 antiDiagAttacks(U64 occ, int pos)
{
	U64 forward = occ & antiDiagMask[pos];
	U64 reverse = _byteswap_uint64(forward);
	forward -= singleBB[pos];
	reverse -= _byteswap_uint64(singleBB[pos]);
	forward ^= _byteswap_uint64(reverse);
	return forward &= antiDiagMask[pos];
}

U64 rookAttack(int from, U64 occupied, U64 enemy, U64 empty)
{
	U64 attack = lineAttacks(occupied, from) | fileAttacks(occupied, from);
	attack &= empty | enemy;
}
//https://cpu.land/slice-dat-time
//https://www.chessprogramming.org/Square_Attacked_By
//https://core.ac.uk/download/572627675.pdf
//https://www.chessprogramming.org/X-ray_Attacks_(Bitboards)
//https://www.chessprogramming.org/Checks_and_Pinned_Pieces_(Bitboards)
//https://www.chessprogramming.org/DirGolem


void Board::getRookMoves(Pieces rook, MoveList& moveList)
{
	U64 rooks = bb[rook];
	while (rooks) {
		int from = bitScanForwardWithReset(rooks);
		U64 attack = lineAttacks(occupied, from) | fileAttacks(occupied, from);
		attack &= (getEnemy(rook) | getEmpty());
		while (attack) {
			int to = bitScanForwardWithReset(attack);
			if ((1ULL << to) & occupied) {
				moveList.moves[moveList.count++] = Move(from, to, rook, getPiece(to), NONE);
			}
			else {
				moveList.moves[moveList.count++] = Move(from, to, rook, EMPTY, NONE);
			}
		}
	}
}

void Board::getBishopMoves(Pieces bishop, MoveList& moveList)
{
	U64 bishops = bb[bishop];
	while (bishops) {
		int from = bitScanForwardWithReset(bishops);
		U64 attack = diagonalAttacks(occupied, from) | antiDiagAttacks(occupied, from);
		attack &= (getEnemy(bishop) | getEmpty());
		while (attack) {
			int to = bitScanForwardWithReset(attack);
			if ((1ULL << to) & occupied) {
				moveList.moves[moveList.count++] = Move(from, to, bishop, getPiece(to), NONE);
			}
			else {
				moveList.moves[moveList.count++] = Move(from, to, bishop, EMPTY, NONE);
			}
		}
	}
}

void Board::getQueenMoves(Pieces queen, MoveList& moveList)
{
	U64 queens = bb[queen];
	while (queens) {
		int from = bitScanForwardWithReset(queens);
		U64 attack = diagonalAttacks(occupied, from) | antiDiagAttacks(occupied, from) | lineAttacks(occupied, from) | fileAttacks(occupied, from);
		attack &= (getEnemy(queen) | getEmpty());
		while (attack) {
			int to = bitScanForwardWithReset(attack);
			if ((1ULL << to) & occupied) {
				moveList.moves[moveList.count++] = Move(from, to, queen, getPiece(to), NONE);
			}
			else {
				moveList.moves[moveList.count++] = Move(from, to, queen, EMPTY, NONE);
			}
		}
	}
}

/* MAGIC BITBOARDS */