#pragma once
#include <vector>
#include "ChessEngine.h"
#include "board.h"
#include "move.h"

class ChessEngine
{
public:
    Board board;
    MoveList moveList;
    std::vector<Position> playedMoves;
    ChessEngine() : moveList(218) { playedMoves.resize(300); board.init(); }

    State gameState() {
        if (board.isCheckmate(moveList.count))
            return (board.isWhiteTurn()) ? BLACK_WIN : WHITE_WIN;
        if (board.isDraw(moveList.count))
            return DRAW;
        return PLAYING;
    }
};