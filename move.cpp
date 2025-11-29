#pragma once
#include "move.h"
 

Move::Move(int from, int to, int piece, int capturedPiece, int specialMove)
	:move(((specialMove & 0xf) << 20) | ((capturedPiece & 0xf) << 16) | ((piece & 0xf) << 12) | ((to & 0x3f) << 6) | (from & 0x3f)) {
}

int Move::getFrom()
{
	return  (move & 0x3f);
}

int Move::getTo()
{
	return ((move >> 6) & 0x3f);
}

int Move::getPiece()
{
	return ((move >> 12) & 0xf);
 }

int Move::getPieceColor()
{
	return pieceColor::getPieceColor((Piece)getPiece());
}

int Move::getPieceGroup()
{
	return (pieceColor::getPieceColor((Piece)getPiece()) == White) ? Whites : Blacks;
}


int Move::getCapturedPiece()
{
	return (move >> 16) & 0xf;
}

int Move::getSpecialMove()
{
	return (move >> 20) & 0xf;
}

bool Move::isCapture() 
{
	return getCapturedPiece() != EMPTY;
}

bool Move::isPromotion()
{
	return ((move >> 20) & 0xf) >= QUEEN_PROM && ((move >> 20) & 0xf) <= KNIGHT_PROM;
}

bool Move::isCastling()
{
	return ((move >> 20) & 0xf) == KING_CASTLING || ((move >> 20) & 0xf) == QUEEN_CASTLING;
}

void Move::setPromotion(int promotionPiece) // can be done maybe better
{
	move &= ~(0xfff << 20);
	move |= (promotionPiece & 0xf) << 20;
}

std::string Move::getStr()
{
	const std::string files[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    std::string str = files[getFrom() % 8] + std::to_string(8 - getFrom() / 8) + files[getTo() % 8] + std::to_string(8 - getTo() / 8);
	if (isPromotion()) {
		if (getSpecialMove() == QUEEN_PROM)
			str.append("q");
		else if (getSpecialMove() == ROOK_PROM)
			str.append("r");
		else if (getSpecialMove() == KNIGHT_PROM)
			str.append("n");
		else if (getSpecialMove() == BISHOP_PROM)
			str.append("b");
	}

	return str;
}

PosInfo::PosInfo(int castlingRights, int epSquare, int halfMoveClock)
	: posInfo(((halfMoveClock & 0x7f) << 10) | ((epSquare & 0x3f) << 4) | (castlingRights & 0xf)) {
}

int PosInfo::getCastlingRights()
{
	return posInfo & 0xf;
}

int PosInfo::getEpSquare()
{
	return (posInfo >> 4) & 0x3f;
}

int PosInfo::getHalfMoveClock()
{
	return (posInfo >> 10) & 0x7f;
}