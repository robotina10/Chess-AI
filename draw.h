#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "constants.h"

class Board;
struct MoveList;

void dragPiece(sf::RenderWindow& win, Board& board, sf::Vector2i pos, int squarePos, Piece pieceType);
void drawPieces(sf::RenderWindow& win, Board& board, bool whiteView);
void drawBoard(sf::RenderWindow& win);
void highlightSquare(sf::RenderWindow& win, Board& board, int squarePos, bool whiteView);
void drawPossibleMoves(sf::RenderWindow& win, MoveList& moveList, int from, bool whiteView);
void drawRanksFiles(sf::RenderWindow& win);