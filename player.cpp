#include "player.h"


void Player::applyMoves(double deltaTime, coordinates viewPosition, coordinates viewSize, coordinates levelSize){
	Entity::applyMoves(deltaTime);
	position.x = max(0.f, position.x);
	position.x = min(float(levelSize.x - size.x), position.x);
	position.y = max(position.y, float(viewPosition.y));
	position.y = min(position.y, float(viewPosition.y + viewSize.y - size.y));
	movementQueue.clear();
}

void Player::shoot(double deltaTime){
	const int velocity = 400;
	cooldown -= deltaTime;
	if (cooldown > 0)
		return;
	cooldown = cooldownTime;
	coordinates bulletSize = { 25, 25 };
	Bullet bullet = Bullet(Sprite::Bullet, { int(position.y), int(position.x - bulletSize.x / 2 + size.x / 2) }, size, { -1, 0 }, velocity, 2, Owner::Player);
	bulletQueue.push_back({ bullet, 0 });
}

Player::Player(Sprite sprite, coordinates position, coordinates size, double cooldownTime, double invicibilityTime, int maxHp) : Entity(sprite, position, size){
	this->cooldown = 0;
	this->cooldownTime = cooldownTime;
	this->invicibilityTime = invicibilityTime;
	this->invicibility = 0;
	this->maxHp = maxHp;
	this->hp = maxHp;
	this->combo = 1;
	this->score = 0;
}


bool Player::dealDamage(){
	if (invicibility > 0)
		return false;
	invicibility = invicibilityTime;
	hp--;
	combo = 1;
	score /= 1.5;
	return true;
}

void Player::addScore(){
	score += combo;
	combo *= 2;
}

void Player::recalculateInvicibilityCooldown(double deltaTime){
	invicibility -= deltaTime;
}

int Player::getCurrentHp() const{
	return hp;
}

int Player::getMaxHp() const{
	return maxHp;
}

void Player::heal(){
	hp = maxHp;
}

int Player::getScore() const{
	return score;
}

Player::Player() : Entity(){
	cooldownTime = 1;
	cooldown = 0;
	hp = 5;
	maxHp = 5;
	invicibilityTime = 2;
	invicibility = 0;
	combo = 1;
	score = 0;
}

vector<pair<Bullet, double>>* Player::getBulletQueue() {
	return &bulletQueue;
}