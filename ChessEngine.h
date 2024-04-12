#pragma once
#include "board.h"
#include "move.h"

class ChessEngine
{
public:
    Board board;
    MoveList moveList;
    MoveList gameMoveList;
    ChessEngine() : moveList(218), gameMoveList(512) { board.setBoard(); board.getMoves(moveList); }
};