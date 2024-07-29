#pragma once
#include <vector>
#include "board.h"
#include "move.h"

class ChessEngine
{
public:
    Board board;
    MoveList moveList;
    std::vector<Board> gamePositions;
    ChessEngine() : moveList(256) { board.setBoard(); board.initAttackArrs(); }

    State gameState() {
        if (board.isCheckmate(moveList.count))
            return (board.isWhiteTurn()) ? BLACK_WIN : WHITE_WIN;
        if (board.isStalemate(moveList.count) || board.isDrawByMaterial() || board.fiftyMoveRule())
            return DRAW;
        return PLAYING;
    }
};