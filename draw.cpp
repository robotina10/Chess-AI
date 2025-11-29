#include "draw.h"
#include "board.h"
#include "move.h"
#include <iostream>
#include <cstdlib>

// Definition of pieceTypes - moved from header to avoid multiple definitions
const std::vector<std::string> pieceTypes = { "bk", "wk", "bq", "wq", "br", "wr", "bb", "wb", "bn", "wn", "bp", "wp" };

void drawPieces(sf::RenderWindow& win, Board& board, bool whiteView)
{
	for (int pieceIndex = 0; pieceIndex < pieceTypes.size(); pieceIndex++) {
		U64 bitboard = board.getBitboard(pieceIndex);
		for (int i = 0; i < 64; i++) {
			if ((bitboard >> i) & 1) {
				sf::Texture texture;
				if (!texture.loadFromFile("img/" + pieceTypes[pieceIndex] + ".png")) {
					std::cout << "Cannot load piece img";
					exit(-1);
				}
				texture.setSmooth(true);
				sf::Sprite piece(texture);

				int x = SIDE * (7 - i % 8) + SIDE / 10;
				int y = SIDE * (7 - i / 8) + SIDE / 10;

				if (whiteView) {
					x = SIDE * (i % 8) + SIDE / 10;
					y = SIDE * (i / 8) + SIDE / 10;
				}
				piece.setPosition(sf::Vector2f(x, y));
				piece.setScale(sf::Vector2f(1.3f, 1.3f));
				win.draw(piece);
			}
		}
	}
}

void drawBoard(sf::RenderWindow& win)
{
	sf::RectangleShape rect(sf::Vector2f(SIDE, SIDE));
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			// SFML 3: setPosition takes a Vector2f instead of two scalars
			rect.setPosition(sf::Vector2f(i * SIDE, j * SIDE));
			if ((i + j) % 2 == 0) {
				rect.setFillColor(WHITE);
			}
			else {
				rect.setFillColor(BLACK);
			}
			win.draw(rect);
		}
	}
}

void highlightSquare(sf::RenderWindow& win, Board& board, int squarePos, bool whiteView)
{
	if (squarePos == -1)
		return;
	sf::RectangleShape rect(sf::Vector2f(SIDE, SIDE));
	if (!whiteView)
		squarePos = 63 - squarePos;
	int y = (squarePos / 8);
	int x = (squarePos % 8);
	// SFML 3: setPosition takes a Vector2f instead of two scalars
	rect.setPosition(sf::Vector2f(x * SIDE, y * SIDE));

	if ((x + y) % 2 == 0)
		rect.setFillColor(HIGHLIGHTED_WHITE);
	else
		rect.setFillColor(HIGHLIGHTED_BLACK);
	win.draw(rect);
}

void drawPossibleMoves(sf::RenderWindow& win, MoveList& moveList, int from, bool whiteView)
{
	sf::RectangleShape rect(sf::Vector2f(SIDE, SIDE));
	for (int i = 0; i < moveList.count; i++) {
		if (moveList.moves[i].getFrom() == from) {
			int to = moveList.moves[i].getTo();
			if (!whiteView) {
				to = 63 - moveList.moves[i].getTo();
			}
			int y = to / 8;
			int x = to % 8;
			rect.setFillColor(sf::Color::Red);
			// SFML 3: setPosition takes a Vector2f instead of two scalars
			rect.setPosition(sf::Vector2f(x * SIDE, y * SIDE));
			if ((moveList.moves[i].getFrom() % 8 + moveList.moves[i].getFrom() | 8 + x + y) % 2 == 0)
				rect.setFillColor(HIGHLIGHTED_WHITE);
			else
				rect.setFillColor(HIGHLIGHTED_BLACK);
			win.draw(rect);
		}
	}
}

void drawRanksFiles(sf::RenderWindow& win)
{
	/*sf::Font font;
	if (!font.loadFromFile("arial.ttf"))
	{
		std::cout << "Failed to load font!\n";
	}
	sf::Text text;
	for (int i = 1; i <= 8; i++) {
		text.setFont(font);
		text.setString("1");
		text.setCharacterSize(8);
		text.setFillColor(sf::Color::Black);
		text.setPosition(SIDE * i - 10, WIN_HEIGHT - 10);
		window.draw(text);
	}*/
}

void dragPiece(sf::RenderWindow& win, Board& board, sf::Vector2i pos, int squarePos, Piece pieceType)
{
	sf::Texture texture;
	if (!texture.loadFromFile("img/" + pieceTypes[pieceType] + ".png")) {
		std::cout << "Cannot load piece img";
		exit(-1);
	}
	texture.setSmooth(true);
	sf::Sprite piece(texture);
	piece.setPosition(sf::Vector2f(pos.x, pos.y));
	piece.setScale(sf::Vector2f(1.8f, 1.8f));
	win.draw(piece);
}
