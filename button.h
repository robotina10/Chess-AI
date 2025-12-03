#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "constants.h"

class Button : public sf::Drawable
{
protected:
	sf::RectangleShape rect;
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const { target.draw(rect); }
public:
	friend class textButton;
	friend class imgButton;

	Button() {}
	Button(sf::Vector2f pos) {
		rect.setPosition(pos);
	}
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
		: Button(pos), text(font, s, charSize)
	{
		text.setPosition(pos);
		text.setFillColor(textColor);
		text.setStyle(sf::Text::Bold);
	}

	void setRect(sf::Vector2f posOffset, sf::Vector2f sizeOffset, sf::Color fillCol, float outlineThickness, sf::Color outlineCol, bool center = false)
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
	imgButton(std::string imgName, sf::Vector2f pos, float scaleX = 1, float scaleY = 1)
		: Button(pos), texture(), sprite(texture)
	{
		if (!texture.loadFromFile(imgName)) {
			std::cout << "Cannot load texture: " << imgName << '\n';
			exit(-1);
		}
		texture.setSmooth(true);

		sprite.setTexture(texture, true);

		sprite.setPosition(pos);
		sprite.setScale(sf::Vector2f(scaleX, scaleY));

		rect.setPosition(pos);
		rect.setSize(sprite.getGlobalBounds().size);

	}

	imgButton(const imgButton& other) : Button(other), texture(other.texture), sprite(other.sprite)
	{
		sprite.setTexture(texture); 
	}

	imgButton& operator=(const imgButton& other) {
		if (this != &other) {
			Button::operator=(other);
			texture = other.texture;
			sprite = other.sprite; 
			sprite.setTexture(texture); 
		}
		return *this;
	}

	void setRect(sf::Vector2f posOffset, sf::Vector2f sizeOffset, sf::Color fillCol, float outlineThickness, sf::Color outlineCol)
	{
		rect.setFillColor(fillCol);
		rect.setOutlineThickness(outlineThickness);
		rect.setOutlineColor(outlineCol);

		sf::Vector2f spriteSize = sprite.getGlobalBounds().size;
		rect.setSize(spriteSize + sizeOffset);

		sf::Vector2f currentPos = sprite.getPosition();
		sf::Vector2f newPos = currentPos + posOffset;

		sprite.setPosition(newPos);
		rect.setPosition(newPos);
	}

	void setPos(sf::Vector2f pos)
	{
		sprite.setPosition(pos);
		rect.setPosition(pos);
	}

	sf::Vector2f getPos()
	{
		return sprite.getPosition();
	}
};