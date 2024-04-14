#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ChessEngine.h"

const int WIN_WIDTH = 800, WIN_HEIGHT = 800;
const int ROWS = 8, COLS = 8;
const int SIDE = WIN_WIDTH / ROWS;

const sf::Color WHITE(255, 255, 255);
const sf::Color BLACK(128, 128, 128);
const sf::Color HIGHLIGHTED_WHITE(225, 225, 225);
const sf::Color HIGHLIGHTED_BLACK(178, 178, 178);

const std::vector<std::string> pieceTypes = { "wk", "wq", "wr", "wb", "wn", "wp","bk", "bq", "br", "bb", "bn", "bp" };

void draw(sf::RenderWindow& window, ChessEngine& chess, int from, int white);
void dragPiece(sf::RenderWindow& window, Board& board, sf::Vector2i pos, int squarePos, Pieces pieceType);