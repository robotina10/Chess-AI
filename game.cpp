#include "game.h"

int calcSquarePos(sf::Vector2i p, bool whiteTurn)
{
    p.x /= SIDE;
    p.y /= SIDE;
    if (p.x >= ROWS)
        p.x = ROWS - 1;
    if (p.y >= COLS)
        p.y = COLS - 1;
    if (p.x < 0)
        p.x = 0;
    if (p.y < 0)
        p.y = 0;
    if (whiteTurn)
        return p.x + p.y * 8;
    return 63 - (p.x + p.y * 8);
}

int getMoveIndex(MoveList& moveList, int from, int to)
{
    for (int i = 0; i < moveList.count; i++) {
        if (from == moveList.moves[i].getFrom() && to == moveList.moves[i].getTo()) {
            return i;
        }
    }
    return -1;
}

void makeMove(sf::RenderWindow &window, ChessEngine &chess, int &from, int to, Pieces &piece, int moveIndex, SpecialMove promotion)
{
    Move move = chess.moveList.moves[moveIndex];
    if (move.isPromotion()) {
        move.setPromotion(promotion);
    }
    chess.gamePositions.push_back(chess.board);
    chess.board.makeMove(move);
    chess.moveList.count = 0;
    chess.board.generateLegalMoves(chess.moveList);
    piece = EMPTY;
    from = -1;
}
