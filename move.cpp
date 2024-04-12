#pragma once
#include "move.h"
#include <iostream>
 

Move::Move(int from, int to, int piece, int capturedPiece, int promotion, int specialMove)
// forgot double pawn push, just all specisal moves
	:move(((specialMove & 0xf) << 24) | ((promotion & 0xf) << 20) | ((capturedPiece & 0xf) << 16) | ((piece & 0xf) << 12) | ((to & 0x3f) << 6) | (from & 0x3f)) {
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
	return ((move >> 16) & 0xf);
}

int Move::getCaptureColor()
{
	return (getCapturedPiece() < 6) ? 12 : 13;
}

int Move::getPromotion()
{
	return (move >> 21) & 0xf;
}

int Move::isCastling() //
{
	return (move >> 25) & 0xf;
}

int Move::isDoublePawnPush() //
{
	return (move >> 25) & 0xf;
}

bool Move::isCapture() 
{
	return getCapturedPiece() != 14; // 14 is EMPTY but it is in board.h
}

void Move::printMove()
{
	std::cout << getFrom();
	std::cout << getTo();
	std::cout << getPiece();
	std::cout << getCapturedPiece();
	std::cout << isCapture();
	std::cout << getPromotion();
	std::cout << isCastling();
	std::cout << isDoublePawnPush();
}