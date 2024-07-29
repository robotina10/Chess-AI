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

int Move::getPieceGroup()
{
	return (pieceColor::getPieceColor((Pieces)getPiece()) == White) ? Whites : Blacks;
}


int Move::getCapturedPiece()
{
	return (move >> 16) & 0xf;
}

int Move::getCaptureGroup()
{
	return (pieceColor::getOpponentGroup((Pieces)getPiece()));
}

int Move::getSpecialMove()
{
	return (move >> 20) & 0xf;
}

bool Move::isCapture() 
{
	return getCapturedPiece() != EMPTY || getSpecialMove() == EN_PASSANT;
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


void Move::printMove()
{
	const char files[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
	//std::cout << "From: " << getFrom() << " To: " << getTo() << "\n";
	if (isPromotion()) {
		if (getSpecialMove() == QUEEN_PROM)
			std::cout << files[getFrom() % 8] << 8 - getFrom() / 8 << files[getTo() % 8] << 8 - getTo() / 8 << "q: ";
		else if (getSpecialMove() == ROOK_PROM)
			std::cout << files[getFrom() % 8] << 8 - getFrom() / 8 << files[getTo() % 8] << 8 - getTo() / 8 << "r: ";
		else if (getSpecialMove() == KNIGHT_PROM)
			std::cout << files[getFrom() % 8] << 8 - getFrom() / 8 << files[getTo() % 8] << 8 - getTo() / 8 << "n: ";
		else if (getSpecialMove() == BISHOP_PROM)
			std::cout << files[getFrom() % 8] << 8 - getFrom() / 8 << files[getTo() % 8] << 8 - getTo() / 8 << "b: ";
	}
	else
		std::cout << files[getFrom() % 8] << 8 - getFrom() / 8  << files[getTo() % 8] << 8 - getTo() / 8 << ": ";
}
