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
	Button(sf::Vector2f pos) {
		rect.setPosition(pos); }
	sf::Vector2f getPos() { return rect.getGlobalBounds().position; }
	sf::Vector2f getSize() { return rect.getGlobalBounds().size; }

	bool isClicked(sf::Vector2i mousePos)
	{
		auto btn = rect.getGlobalBounds();
		return mousePos.x >= btn.position.x && mousePos.x <= btn.position.x + btn.size.x &&
		       mousePos.y >= btn.position.y && mousePos.y <= btn.position.y + btn.size.y;
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
	textButton(std::string s, sf::Font& font, sf::Vector2f pos, int charSize, sf::Color textColor)
		// SFML 3: sf::Text constructor now takes (font, string, size)
		: Button(pos), text(font, s, charSize)
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
			size = text.getGlobalBounds().size;
			size.x = (WIN_WIDTH - size.x) / 2;
			pos = text.getGlobalBounds().position;
			pos.x = size.x;
			text.setPosition(pos);
		}

		size = text.getGlobalBounds().size;
		rect.setSize(size + sizeOffset);

		pos = text.getGlobalBounds().position;
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
	imgButton(std::string imgName, sf::Vector2f pos, float scaleX=1, float scaleY=1) : Button(pos), texture(), sprite(texture)
	{
		if (!texture.loadFromFile(imgName)) {
			std::cout << "Cannot load texture: " << imgName << '\n';
			exit(-1);
		}
		texture.setSmooth(true);
	sprite.setTexture(texture);
		sprite.setPosition(pos);
		// SFML 3: Transformable::setScale now takes a Vector2f
		sprite.setScale(sf::Vector2f(scaleX, scaleY));
	}

	void setRect(sf::Vector2f posOffset, sf::Vector2f sizeOffset, sf::Color fillCol, float outlineThickness, sf::Color outlineCol)
	{
		rect.setFillColor(fillCol);
		rect.setOutlineThickness(outlineThickness);
		rect.setOutlineColor(outlineCol);
		sf::Vector2f size = sprite.getGlobalBounds().size;
		rect.setSize(size + sizeOffset);

		sf::Vector2f pos = sprite.getGlobalBounds().position;
		rect.setPosition(pos + posOffset);
		sprite.setPosition(pos + posOffset);

	}

	void setPos(sf::Vector2f pos)
	{
		sprite.setPosition(pos);
	}

	sf::Vector2f getPos()
	{
		return sprite.getPosition();
	}
};
