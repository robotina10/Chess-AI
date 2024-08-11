#pragma once
#include <SFML/Graphics.hpp>

const int startingSquareOfImage = 17; // must be <= than 63 - 24

class Button
{
private:
	sf::Texture texture;
	sf::Sprite sprite;
public:
	Button(std::string imgName)
	{ 
		if (!texture.loadFromFile(imgName)) {
			std::cout << "Cannot load " << imgName << '\n';
			exit(-1);
		}
		//texture.setSmooth(true);
		sprite.setTexture(texture);
		sprite.setPosition(sf::Vector2f(startingSquareOfImage % 8 * SIDE, startingSquareOfImage / 8 * SIDE));
		sprite.setScale(sf::Vector2f(1.168f, 0.89f));
	}

	SpecialMove pieceClicked(int square)
	{
		switch (square) {
		case startingSquareOfImage:
			return QUEEN_PROM;
		case startingSquareOfImage + 8:
			return KNIGHT_PROM;
		case startingSquareOfImage + 16:
			return ROOK_PROM;
		case startingSquareOfImage + 24:
			return BISHOP_PROM;
		default:
			return NONE;
		}
	}

	void drawAndDisplay(sf::RenderWindow& window) { window.draw(sprite); window.display(); }
};