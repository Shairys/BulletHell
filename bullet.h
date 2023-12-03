#pragma once
#include "entity.h"

class Bullet : public Entity {
	double lifespawn;
	vector<pair<Bullet, double>> bulletQueue;
	vector<Bullet> explodeQueue;
	int velocity;
	coordinates angle;
	void explode();
	Owner owner;
	
public:
	vector<pair<Bullet, double>>* getBulletQueue();
	using Entity::Entity;
	Bullet(const Bullet& bullet);
	Bullet();
	Bullet(Sprite sprite, coordinates position, coordinates size, coordinates angle, int velocity, double lifespawn, Owner owner);
	Bullet& operator = (const Bullet& bullet);
	void addExplosion(Bullet bullet);
	bool calculateBullet(double deltaTime);
	Owner getOwner() const;
	void setLifespawn(double time);
	
};