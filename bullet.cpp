#include "bullet.h"

Bullet::Bullet(const Bullet& bullet) : Bullet(){
	this->sprite = bullet.sprite;
	this->size = bullet.size;
	this->position.x = bullet.position.x;
	this->position.y = bullet.position.y;
	this->lifespawn = bullet.lifespawn;
	this->angle = bullet.angle;
	this->velocity = bullet.velocity;
	this->bulletQueue = bullet.bulletQueue;
	this->explodeQueue = bullet.explodeQueue;
	this->owner = bullet.owner;
}


Bullet::Bullet(){
	sprite = Sprite(0);
	position = { 0, 0 };
	size = { 50, 50 };
	lifespawn = 1;
	angle = { 1, 0 };
	velocity = 50;
	owner = Owner::System;

}

Bullet::Bullet(Sprite sprite, coordinates position, coordinates size, coordinates angle, int velocity, double lifespawn, Owner owner) : Entity(sprite, position, size) {
	this->lifespawn = lifespawn;
	this->angle = angle;
	this->velocity = velocity;
	this->owner = owner;
}

Bullet& Bullet::operator=(const Bullet& bullet){
	this->sprite = bullet.sprite;
	this->size = bullet.size;
	this->position.x = bullet.position.x;
	this->position.y = bullet.position.y;
	this->lifespawn = bullet.lifespawn;
	this->angle = bullet.angle;
	this->velocity = bullet.velocity;
	this->bulletQueue = bullet.bulletQueue;
	this->explodeQueue = bullet.explodeQueue;
	this->owner = bullet.owner;
	return (*this);
}

void Bullet::addExplosion(Bullet bullet){
	explodeQueue.push_back(bullet);
}


bool Bullet::calculateBullet(double deltaTime){
	lifespawn -= deltaTime;
	if (lifespawn < 0) {
		explode();
		return false;
	}
	queueMove(angle, velocity);
	applyMoves(deltaTime);
	return true;
}

Owner Bullet::getOwner() const{
	return owner;
}

void Bullet::setLifespawn(double time){
	lifespawn = time;
}

void Bullet::explode() { //"wybuch" pocisków w ró¿ne strony
	for (int i = 0; i < explodeQueue.size(); i++) {
		Bullet bullet = explodeQueue[i];
		bullet.position = getPosition();
		bullet.size = { getSize().y / 2, getSize().x / 2 };
		bulletQueue.push_back({ bullet, 0 });
	}
}

vector<pair<Bullet, double>>* Bullet::getBulletQueue() {
	return &bulletQueue;
}


