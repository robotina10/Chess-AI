#include "game.h"

int calcSquarePos(sf::Vector2i p)
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
    return p.x + p.y * 8;
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

void makeMove(sf::RenderWindow &window, ChessEngine &chess, int &from, Pieces &piece)
{
    int to = calcSquarePos(sf::Mouse::getPosition(window));
    int i = getMoveIndex(chess.moveList, from, to);
    if (i == -1) {
        from = to;
        return;
    }
    chess.board.makeMove(chess.moveList.moves[i]);
    chess.gameMoveList.moves[chess.gameMoveList.count++] = (chess.moveList.moves[i]);
    chess.moveList.count = 0;
    chess.board.getMoves(chess.moveList);
    piece = EMPTY;
    from = -1;
}
