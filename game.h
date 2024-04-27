#pragma once
#include "draw.h"
#include "move.h"
#include <SFML/Graphics.hpp>

int calcSquarePos(sf::Vector2i pos, bool whiteTurn);
int getMoveIndex(MoveList& moveList, int from, int to);
void makeMove(sf::RenderWindow& window, ChessEngine& chess, int& from, int to, Pieces& piece, int i, SpecialMove promotion);
