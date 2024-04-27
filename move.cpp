#pragma once
#include "move.h"
#include <iostream>
 

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
	return (getPiece() < 6) ? 12 : 13;
}

int Move::getCapturedPiece()
{
	return (move >> 16) & 0xf;
}

int Move::getCaptureColor()
{
	return (getCapturedPiece() < 6) ? 12 : 13;
}

int Move::getEnPassant()
{
	return (move >> 20) & EN_PASSANT;
}

int Move::getPromotion()
{
	return (move >> 20) & 0xf;
}

int Move::getCastling()
{
	return (move >> 20) & 0xf;
}

bool Move::isCapture() 
{
	return getCapturedPiece() != 14; // 14 is EMPTY but it is in board.h
}

bool Move::isPromotion()
{
	return ((move >> 20) & 0xf) == GENERIC_PROM;
}

void Move::setPromotion(int promotionPiece)
{
	move &= ~(0xfff << 20);
	move |= (promotionPiece & 0xf) << 20;
}


void Move::printMove()
{
	std::cout << "from:" << getFrom() << "\n";
	std::cout << "to: " << getTo() << "\n";
	std::cout << "piece: " << getPiece() << "\n";
	std::cout << "capturedPiece: " << getCapturedPiece() << "\n";
	std::cout << "isCapture: " << isCapture() << "\n";
	//std::cout << getPromotion() << "\n";
}