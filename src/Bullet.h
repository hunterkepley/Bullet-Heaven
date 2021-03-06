#include <iostream>
#include <SFML/Graphics.hpp>
#include "ParticleEmitter.h"
#include <memory>

#ifndef BULLET_H
#define BULLET_H

class Bullet{
    public:
	Bullet(sf::Vector2f npos, sf::Vector2f size, sf::Vector2f nspeed, sf::Color ncolor);

	Bullet(sf::Vector2f npos, sf::Vector2f size, sf::Vector2f nspeed, sf::Color ncolor, sf::Color noutlineColor);

	sf::Vector2f pos;
	sf::Vector2f speed;
	sf::Color color;
	sf::RectangleShape shape;

	void update();
	void render(sf::RenderWindow& window);

	bool deleteThis;

    private:

};

#endif
