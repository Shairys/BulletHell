#pragma once
#include "entity.h"
#include "bullet.h"


class Enemy : public Entity{

	double cooldown, cooldownTime;
	vector<pair<Bullet, double>> bulletQueue;
	void generatePattern(AttackType attackType, double delay, int offset, int numberOfShots, bool explosion = true, shotTiming timing = shotTiming::Instant);
	int enemyAI;
	coordinates moveTarget;
	bool currentlyMoving;
	double lastMoveTime;
	coordinates levelSize;
	int maxHp, hp;

public:
	vector<pair<Bullet, double>>* getBulletQueue();
	void shoot(double deltaTime);
	void move(double deltaTime);
	using Entity::Entity;
	Enemy(Sprite sprite, coordinates position, coordinates size, double cooldownTime, int enemyAI, coordinates levelSize, int maxHp);
	Enemy();
	int getCurrentHp() const;
	int getMaxHp() const;
	void processAI(double deltaTime);
	void dealDamage();
};