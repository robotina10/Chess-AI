#pragma once
#include "draw.h"
#include "move.h"
#include <SFML/Graphics.hpp>

int calcSquarePos(sf::Vector2i pos);
void makeMove(sf::RenderWindow& window, ChessEngine& chess, int& from, Pieces& piece);
