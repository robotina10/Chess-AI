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
	return pieceColor::getPieceColor((Pieces)getPiece());
}

int Move::getCapturedPiece()
{
	return (move >> 16) & 0xf;
}

int Move::getCaptureColor()
{
	return pieceColor::getPieceColor((Pieces)getCapturedPiece());
}

int Move::getPromotion()
{
	return ((move >> 20) & 0xf) == GENERIC_PROM;
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
	return ((move >> 20) & 0xf) == GENERIC_PROM;
}

void Move::setPromotion(int promotionPiece) // can be done maybe better
{
	move &= ~(0xfff << 20);
	move |= (promotionPiece & 0xf) << 20;
}


void Move::printMove()
{
	std::cout << "from:" << (getSpecialMove() & KING_CASTLING) << "\n";
}
