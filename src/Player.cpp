#include <iostream>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "ParticleEmitter.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Shield.h"
#include <memory>

using namespace std;

Player::Player(sf::Vector2f npos, sf::Vector2f nsize, sf::Color nbaseColor):
    pos(npos),
    size(nsize),
    baseColor(nbaseColor),
    alive(true){

	lives = 2;

	cR = 0;
	cG = 0;
	cB = 255;

	baseColor = sf::Color(cR, cG, cB);

	colorBool = true;

	drawEngineParticles = true;

	playerLifeAnimationTimer = 30;
	playerLifeAnimationTimerBase = playerLifeAnimationTimer;

	canRender = true;

	shieldBar.setFillColor(sf::Color(100, 255, 100));
	shieldBar.setSize(sf::Vector2f(size.x, 4));
	shieldBar.setPosition(sf::Vector2f(pos.x, pos.y + (size.y - (shieldBar.getSize().y + (shieldBar.getSize().y/2)))));

	playerShape.setFillColor(baseColor);
	playerShape.setSize(size);
	playerShape.setPosition(pos);

	playerEngineXC = 3.f; // Constants for the divisions I use for placing the ship's parts, to keep it the same any time I change the size
	playerEngineYC = 6.f;	

	playerEngine.setFillColor(sf::Color(100, 100, 100)); // Obv.
	playerEngine.setSize(sf::Vector2f(playerShape.getSize().x/playerEngineXC,playerShape.getSize().y/playerEngineYC)); // sets size!
	playerEngine.setPosition(sf::Vector2f(pos.x+((playerShape.getSize().x/2)-(playerEngine.getSize().x/2)), pos.y+playerShape.getSize().y));

	playerGlassXC = 3.5f; // Rest of the stuff is the same
	playerGlassYC = 2.8f;

	playerGlass.setFillColor(sf::Color::Cyan);
	playerGlass.setSize(sf::Vector2f(playerShape.getSize().x/playerGlassXC,playerShape.getSize().y/playerGlassYC));
	playerGlass.setPosition(sf::Vector2f(pos.x+((playerShape.getSize().x/2)-(playerGlass.getSize().x/2)), pos.y+playerGlassYC));

	playerGunXC = 7.f; 
	playerGunYC = 5.6f;	

	playerGun.setFillColor(sf::Color(200, 200, 200)); // Obv.
	playerGun.setSize(sf::Vector2f(playerShape.getSize().x/playerGunXC,playerShape.getSize().y/playerGunYC)); // sets size!
	playerGun.setPosition(sf::Vector2f(pos.x+((playerShape.getSize().x/2)-(playerGun.getSize().x/2)), pos.y-playerGun.getSize().y));


	speed = 4.f; // Basically gamespeed, most movements [like stars] are based off of this speed to give the effect of travelling through space without moving the camera, minimizing math to be done when updating and overall making programming/the game go much smoother.

	pe1.init(sf::Vector2f(playerEngine.getPosition().x + (playerEngine.getSize().x/2 + 1), playerEngine.getPosition().y), sf::Vector2f(speed, speed), sf::Vector2f(2.75f, 2.75f), sf::Vector2f(15, 15), sf::Vector2f(1.f, 1.f), 30.0f, 15.f, 1, PET_DOWN, sf::Color(200, 200, 25)); // Create an particle emitter, with color orange.

	pe2.init(sf::Vector2f(playerEngine.getPosition().x + (playerEngine.getSize().x/2 + 1), playerEngine.getPosition().y), sf::Vector2f(speed, speed), sf::Vector2f(2.75f, 2.75f), sf::Vector2f(10, 10), sf::Vector2f(1.f, 1.f), 30.f, 15.f, 1, PET_DOWN, sf::Color(255, 25, 25)); // Create an particle emitter, with color red.

	bulletTimerBase = 40;
	bulletTimer = bulletTimerBase;

	shieldBool = false;

    }

void Player::render(sf::RenderWindow& window){
    if(alive){
	for(unsigned int i = 0; i < bullets.size(); i++){
	    bullets[i].render(window);
	}
	if(canRender){
	    pe2.render(window);
	    pe1.render(window);
	    window.draw(playerShape);
	    window.draw(playerGlass);
	    window.draw(playerEngine);
	    window.draw(playerGun);
	    window.draw(shieldBar);

	    if(shieldBool){
		shield.render(window);
	    }
	}
    }
}

void Player::update(sf::View& view, int rX, int rY, bool hasFocus){
    if(alive){
	shieldTimerBase = 220; // HOW LONG SHIELD LASTS
	shieldCooldownBase = 650; // HOW LONG THE SHIELD COOLDOWN IS
	// This is all the player's ship parts, and their positions. Makes it easy to keep track of them and not have to do much math to place'em.
	playerShape.setPosition(pos); // All the position setting.
	playerEngine.setPosition(sf::Vector2f(pos.x+((playerShape.getSize().x/2)-(playerEngine.getSize().x/2)), pos.y+playerShape.getSize().y));
	playerGlass.setPosition(sf::Vector2f(pos.x+((playerShape.getSize().x/2)-(playerGlass.getSize().x/2)), pos.y+playerGlassYC));
	playerGun.setPosition(sf::Vector2f(pos.x+((playerShape.getSize().x/2)-(playerGun.getSize().x/2)), pos.y-playerGun.getSize().y));
	shieldBar.setPosition(sf::Vector2f(pos.x, pos.y + (size.y - (shieldBar.getSize().y + (shieldBar.getSize().y/2)))));
	shieldBar.setSize(sf::Vector2f(((shieldCooldown/(shieldCooldownBase/100.0f))/100.0f) * size.x, 4)); // Retarded math to make the shield cooldown bar fit on the ship.
	if(bulletTimer > 0){
	    bulletTimer--;
	}
	for(unsigned int i = 0; i < bullets.size(); i++){
	    bullets[i].update();
	    if(bullets[i].deleteThis) bullets.erase(bullets.begin() + i);
	}
	if(isSpacebar()){
	    if(bulletTimer <= 0){
		Bullet bullet(playerGun.getPosition(), playerGun.getSize(), sf::Vector2f(0, -speed*2.3f), sf::Color::Red);
		bullets.push_back(bullet);
		bulletTimer = bulletTimerBase;
	    }
	}
	if(isPressingE() && shieldCooldown <= 0 && alive){ // Controls the shield.

	    shieldBool = true;

	    shieldTimer = shieldTimerBase;

	    shieldCooldown = shieldCooldownBase;
	}

	if(shieldCooldown > 0){
	    shieldCooldown--;
	}

	if(shieldBool == true){ // Shieeeeeeeeeeeeeeeeeeeeeeeeeeeeeelddd!

	    shieldTimer--;
	    if(shieldTimer <= (shieldTimerBase/2) - (shield.size.y/2)){
		shield.shape.setOutlineColor(sf::Color::White);
		shield.canChangeColor = false;
		if(shieldTimer <= shield.size.y - size.y){

		    if(shield.size.x >= 0){
			shield.size.x--; // This makes the shield noticable when it's about to die so you aren't surprised.
		    }
		    if(shield.size.y >= 0){
			shield.size.y--;
		    }

		    if(shieldTimer <= 0){
			shield.size = shield.sizeBase;
			shield.canChangeColor = true;
			shieldBool = false;
			shieldTimer = shieldTimerBase;
		    }
		}
	    }

	}

	shield.pos = sf::Vector2f((pos.x + (size.x/2.0f)) - (shield.size.x/2.0f), (pos.y + (size.y/2.0f)) - (shield.size.y/2.0f));
	shield.update();

	if(playerShape.getSize().x != size.x || playerShape.getSize().y != size.y){ // If the ship changes size, change
	    playerShape.setSize(size);
	    playerEngine.setSize(sf::Vector2f(playerShape.getSize().x/playerEngineXC,playerShape.getSize().y/playerEngineYC));
	    playerGlass.setSize(sf::Vector2f(playerShape.getSize().x/playerGlassXC,playerShape.getSize().y/playerGlassYC));
	    playerGun.setSize(sf::Vector2f(playerShape.getSize().x/playerGunXC,playerShape.getSize().y/playerGunYC)); // sets size!
	}
	if(isGoingLeft() && hasFocus){ // Basic movement
	    pos.x -= speed;
	}
	else if(isGoingRight() && hasFocus){
	    pos.x += speed;
	}
	if(isGoingUp() && hasFocus){
	    pos.y -= speed;
	}
	else if(isGoingDown() && hasFocus){
	    pos.y += speed;
	}
	if(isGoingUp() && !drawEngineParticles){
	    drawEngineParticles = true;
	    pe1.canMakeParticles = true;
	    pe2.canMakeParticles = true;
	}
	else if(!isGoingUp() && drawEngineParticles)
	    drawEngineParticles = false;

	if(!drawEngineParticles){
	    pe1.canMakeParticles = false;
	    pe2.canMakeParticles = false;
	}

	if(pos.x <= 0) // Make sure you don't leave the map :O
	    pos.x += speed;
	if(pos.x + size.x >= rX)
	    pos.x -= speed;
	if(pos.y <= 0)
	    pos.y += speed;
	if(pos.y + size.y >= rY)
	    pos.y -= speed;

	pe1.pos = sf::Vector2f(playerEngine.getPosition().x + (playerEngine.getSize().x/2 + 1) - pe1.baseSize.x, playerEngine.getPosition().y + playerEngine.getSize().y);
	pe2.pos = sf::Vector2f(playerEngine.getPosition().x + (playerEngine.getSize().x/2 + 1) - pe2.baseSize.y, playerEngine.getPosition().y + playerEngine.getSize().y);

	pe1.update(speed);
	pe2.update(speed);
    }
}

bool Player::isGoingLeft(){ // Input handlers, makes it quicker & easier on me.
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
	return true;
    }
    else{
	return false;
    }
}

bool Player::isGoingRight(){
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
	return true;
    }
    else{
	return false;
    }
}

bool Player::isGoingUp(){
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
	return true;
    }
    else{
	return false;
    }
}

bool Player::isGoingDown(){
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
	return true;
    }
    else{
	return false;
    }
}

bool Player::isSpacebar(){
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
	return true;
    }
    else{
	return false;
    }
}

bool Player::isPressingE(){
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
	return true;
    }
    else{
	return false;
    }
}
