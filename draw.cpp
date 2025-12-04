#include "draw.h"
#include "board.h"
#include "move.h"
#include <iostream>
#include <cstdlib>

const std::vector<std::string> pieceTypes = { "bk", "wk", "bq", "wq", "br", "wr", "bb", "wb", "bn", "wn", "bp", "wp" };

void drawPieces(sf::RenderWindow& win, Board& board, bool whiteView, int excludeSquare)
{
	for (int pieceIndex = 0; pieceIndex < pieceTypes.size(); pieceIndex++) {
		U64 bitboard = board.getBitboard(pieceIndex);
		for (int i = 0; i < 64; i++) {
			if (i == excludeSquare) 
				continue;
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

void highlightSquare(sf::RenderWindow& win, Board& board, int squarePos, bool whiteView, sf::Color whiteCol, sf::Color blackCol)
{
	if (squarePos == -1)
		return;
	sf::RectangleShape rect(sf::Vector2f(SIDE, SIDE));
	if (!whiteView)
		squarePos = 63 - squarePos;
	int y = (squarePos / 8);
	int x = (squarePos % 8);

	rect.setPosition(sf::Vector2f(x * SIDE, y * SIDE));

	if ((x + y) % 2 == 0)
		rect.setFillColor(whiteCol);
	else
		rect.setFillColor(blackCol);
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

			rect.setPosition(sf::Vector2f(x * SIDE, y * SIDE));
			if ((moveList.moves[i].getFrom() % 8 + moveList.moves[i].getFrom() | 8 + x + y) % 2 == 0)
				rect.setFillColor(HIGHLIGHTED_WHITE);
			else
				rect.setFillColor(HIGHLIGHTED_BLACK);
			win.draw(rect);
		}
	}
}

void drawRanksFiles(sf::RenderWindow& win, bool whiteView)
{
	static sf::Font font;
	static bool loaded = false;
	if (!loaded)
	{
		if (!font.openFromFile("font/Poppins-Bold.ttf"))
		{
			std::cout << "Failed to load font: font/Poppins-Bold.ttf\n";
		}
		loaded = true;
	}

	sf::Text text(font);
	text.setCharacterSize(SIDE / 5);

	for (int r = 0; r < 8; r++) {
		std::string label = whiteView ? std::to_string(8 - r) : std::to_string(r + 1);
		text.setString(label);

		text.setPosition(sf::Vector2f(3.f, static_cast<float>(r * SIDE) + 3.f));

		if (r % 2 == 0)
			text.setFillColor(BLACK);
		else
			text.setFillColor(WHITE);

		win.draw(text);
	}

	for (int c = 0; c < 8; c++) {
		char f = whiteView ? ('a' + c) : ('h' - c);
		std::string label(1, f);
		text.setString(label);

		sf::FloatRect bounds = text.getLocalBounds();

		float xPos = static_cast<float>((c + 1) * SIDE) - bounds.size.x - 3.f;
		float yPos = static_cast<float>(8 * SIDE) - bounds.size.y - 13.f;

		text.setPosition(sf::Vector2f(xPos, yPos));

		if ((c + 7) % 2 == 0)
			text.setFillColor(BLACK);
		else
			text.setFillColor(WHITE);

		win.draw(text);
	}
}

void dragPiece(sf::RenderWindow& win, Board& board, sf::Vector2i pos, int squarePos, int pieceIndex)
{
	if (pieceIndex < 0 || pieceIndex >= pieceTypes.size()) return;

	sf::Texture texture;
	if (!texture.loadFromFile("img/" + pieceTypes[pieceIndex] + ".png")) {
		std::cout << "Cannot load piece img";
		exit(-1);
	}
	texture.setSmooth(true);
	sf::Sprite piece(texture);

	piece.setOrigin({ SIDE / 2.0f, SIDE / 2.0f });
	piece.setPosition(sf::Vector2f(pos.x, pos.y));
	piece.setScale(sf::Vector2f(1.3f, 1.3f));
	win.draw(piece);
}