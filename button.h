#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "constants.h"

class Button : public sf::Drawable
{
	sf::RectangleShape rect;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const { target.draw(rect); }
public:
	friend class textButton;
	friend class imgButton;

	Button() {}
	Button(sf::Vector2f pos) { rect.setPosition(pos); }
	sf::Vector2f getPos() { return rect.getGlobalBounds().getPosition(); }
	sf::Vector2f getSize() { return rect.getGlobalBounds().getSize(); }

	bool isClicked(sf::Vector2i mousePos)
	{
		sf::FloatRect btn = rect.getGlobalBounds();
		return mousePos.x >= btn.left && mousePos.x <= btn.left + btn.width && mousePos.y >= btn.top && mousePos.y <= btn.top + btn.height;
	}
};

class textButton : public Button
{
	sf::Text text;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(rect);
		target.draw(text);
	}
public:
	textButton() {}
	textButton(std::string s, sf::Font& font, sf::Vector2f pos, int charSize, sf::Color textColor) : Button(pos), text(s, font, charSize)
	{
		text.setPosition(pos);
		text.setFillColor(textColor);
		text.setStyle(sf::Text::Bold); //
	}

	void setRect(sf::Vector2f posOffset, sf::Vector2f sizeOffset, sf::Color fillCol, float outlineThickness, sf::Color outlineCol, bool center=false)
	{
		rect.setFillColor(fillCol);
		rect.setOutlineThickness(outlineThickness);
		rect.setOutlineColor(outlineCol);

		sf::Vector2f size;
		sf::Vector2f pos;
		if (center) {
			size = text.getGlobalBounds().getSize();
			size.x = (WIN_WIDTH - size.x) / 2;
			pos = text.getGlobalBounds().getPosition();
			pos.x = size.x;
			text.setPosition(pos);
		}

		size = text.getGlobalBounds().getSize();
		rect.setSize(size + sizeOffset);

		pos = text.getGlobalBounds().getPosition();
		rect.setPosition(pos + posOffset);
	}
};

class imgButton : public Button
{
	sf::Texture texture;
	sf::Sprite sprite;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(rect);
		target.draw(sprite);
	}
public:
	imgButton() {}
	imgButton(std::string imgName, sf::Vector2f pos, int scaleX=1, int scaleY=1) : Button(pos)
	{
		if (!texture.loadFromFile(imgName)) {
			std::cout << "Cannot load texture: " << imgName << '\n';
			exit(-1);
		}
		texture.setSmooth(true);
		sprite.setTexture(texture);
		sprite.setPosition(pos);
		sprite.setScale(scaleX, scaleY);
	}

	void setRect(sf::Vector2f posOffset, sf::Vector2f sizeOffset, sf::Color fillCol, float outlineThickness, sf::Color outlineCol)
	{
		rect.setFillColor(fillCol);
		rect.setOutlineThickness(outlineThickness);
		rect.setOutlineColor(outlineCol);
		sf::Vector2f size = sprite.getGlobalBounds().getSize();
		rect.setSize(size + sizeOffset);

		sf::Vector2f pos = sprite.getGlobalBounds().getPosition();
		rect.setPosition(pos + posOffset);
		sprite.setPosition(pos + posOffset);

	}
};

//promotion button

/*const int startingSquareOfImage = 17; // must be <= than 63 - 24  

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
};*/