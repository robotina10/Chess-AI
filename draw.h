#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ChessEngine.h"
#include "constants.h"

void draw(sf::RenderWindow& win, ChessEngine& chess, int from, bool whiteView);
void dragPiece(sf::RenderWindow& win, Board& board, sf::Vector2i pos, int squarePos, Pieces pieceType);