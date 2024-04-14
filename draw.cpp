#include "draw.h"

void drawPieces(sf::RenderWindow& window, Board &board, int white)
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

				if (white) {
				    x = SIDE * (i % 8) + SIDE / 10;
				    y = SIDE * (i / 8) + SIDE / 10;

				}
				piece.setPosition(sf::Vector2f(x, y));
				piece.setScale(sf::Vector2f(1.3f, 1.3f));
				window.draw(piece);
			}
		}
	}
}

void drawBoard(sf::RenderWindow &window)
{
	sf::RectangleShape rect(sf::Vector2f(SIDE, SIDE));
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			rect.setPosition(i * SIDE, j * SIDE);
			if ((i + j) % 2 == 0) {
				rect.setFillColor(WHITE);
			}
			else {
				rect.setFillColor(BLACK);
			}
			window.draw(rect);
		}
	}
}

void highlightSquare(sf::RenderWindow& window, Board& board, int squarePos)
{
	if (squarePos == -1)
		return;
	sf::RectangleShape rect(sf::Vector2f(SIDE, SIDE));
	int y = (squarePos / 8);
	int x = (squarePos % 8);
	rect.setPosition(x * SIDE, y * SIDE);
	if ((x + y) % 2 == 0)
		rect.setFillColor(HIGHLIGHTED_WHITE);
	else 
		rect.setFillColor(HIGHLIGHTED_BLACK);
	window.draw(rect);
}

void drawPossibleMoves(sf::RenderWindow& window, MoveList &moveList, int from)
{
	sf::RectangleShape rect(sf::Vector2f(SIDE, SIDE));
	for (int i = 0; i < moveList.count; i++) {
		if (moveList.moves[i].getFrom() == from) {
			int y = moveList.moves[i].getTo() / 8;
			int x = moveList.moves[i].getTo() % 8;
			rect.setPosition(x * SIDE, y * SIDE);
			rect.setFillColor(sf::Color::Red);
			/*if ((move.getFrom() + move.getTo()) % 2 == 0)
				rect.setFillColor(HIGHLIGHTED_WHITE);
			else
				rect.setFillColor(HIGHLIGHTED_BLACK);*/
			window.draw(rect);
		}
	}
}

void drawRanksFiles(sf::RenderWindow& window)
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

void dragPiece(sf::RenderWindow& window, Board& board, sf::Vector2i pos, int squarePos, Pieces pieceType)
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
	window.draw(piece);
}

void draw(sf::RenderWindow &window, ChessEngine &chess, int from, int white)
{
	window.clear();
	drawBoard(window);
	highlightSquare(window, chess.board, from);
	drawPossibleMoves(window, chess.moveList, from);
	drawPieces(window, chess.board, white);
	drawRanksFiles(window);
	window.display();
}