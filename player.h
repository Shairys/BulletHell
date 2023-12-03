#pragma once
#include "utils.h"
#include "vector.h"
#include <math.h> 
#include <iostream>
#include <stdio.h>
#include "sprites.h"
#include "entity.h"
#include "bullet.h"
#include "config.h"

class Player : public Entity{
	vector<pair<Bullet, double>> bulletQueue;
	double cooldown, cooldownTime;
	double invicibility, invicibilityTime;
	int hp, maxHp;
	int combo;
	int score;

public:
	vector<pair<Bullet, double>>* getBulletQueue();
	void applyMoves(double deltaTime, coordinates viewPosition, coordinates viewSize, coordinates levelSize);
	void shoot(double deltaTime);
	using Entity::Entity;
	Player(Sprite sprite, coordinates position, coordinates size, double cooldownTime, double invicibilityTime, int maxHp);
	bool dealDamage();
	void addScore();
	void recalculateInvicibilityCooldown(double deltaTime);
	int getCurrentHp() const;
	int getMaxHp() const;
	void heal();
	int getScore() const;
	Player();
};

