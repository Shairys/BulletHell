#include "enemy.h"

void Enemy::shoot(double deltaTime){
	cooldown -= deltaTime;
	if (cooldown > 0)
		return;
	if (enemyAI == 1) {
		int attack = rand() % 10;
		double offset = 2.f * 3.1415 * (rand() % 100) / 100;
		if(attack < 6)
			generatePattern(AttackType::Simple, 0, offset * 2, 2, false);
		else if(attack < 7)
			generatePattern(AttackType::CrossAttack, 0.2, offset, 8, true);
		else if (attack == 8)
			generatePattern(AttackType::Circular, 1, offset * 2, 16, true, shotTiming::Instant);
		else
			generatePattern(AttackType::Big, 2, offset * 2, 16, false, shotTiming::Delayed);
		cooldown = 1;
	}
	else if (enemyAI == 2) {
		int attack = rand() % 20;
		double offset = 2.f * 3.1415 * (rand() % 100) / 100;
		if(attack < 12)
			generatePattern(AttackType::Simple, 1, offset * 2, 8, true);
		else if(attack < 14)
			generatePattern(AttackType::Simple, 2, offset * 2, 16);
		else if (attack < 16)
			generatePattern(AttackType::Circular, 0, offset, 4);
		else if (attack < 18)
			generatePattern(AttackType::Circular, 3, offset * 2, 32);
		else
			generatePattern(AttackType::BeamAttack, 0.2, offset, 8);
		cooldown = 1;
	}
	else if (enemyAI == 3) {
		cooldown = (rand() % 15) / 10.f;
		double delay = (rand() % 15) / 20.f + 0.3f;
		double offset = 2.f * 3.1415 * (rand() % 100) / 100;
		double numberOfShots = rand() % 5 + 1;
		bool explosion = bool( int(rand() % 100)/95);
		AttackType attackType = AttackType(max((rand() % 15) - 15 + int(AttackType::Size), 0));
		generatePattern(attackType, delay, offset, numberOfShots, explosion, shotTiming::Delayed);
	}
}

void Enemy::move(double deltaTime) {
	if (enemyAI == 2 || enemyAI == 3)
		return;
	if (currentlyMoving) {
		const int speed = 300;
		int dirX = moveTarget.x - position.x;
		double len = sqrt(dirX * dirX);
		coordinates angle = { 0, int(dirX / len) };
		queueMove(angle, speed);
		applyMoves(deltaTime);
		if (len < 30) {
			currentlyMoving = false;
			lastMoveTime = 0;
		}
	}
	else {
		lastMoveTime += deltaTime;
		if (lastMoveTime > 1) {
			int targetX = position.x + (rand() % int(size.x) * 6) - size.x * 3;
			targetX = max(targetX, 0);
			targetX = min(targetX, int(levelSize.x - size.x));
			currentlyMoving = true;
			moveTarget = { 0, targetX };
		}
	}
}

Enemy::Enemy(Sprite sprite, coordinates position, coordinates size, double cooldownTime, int enemyAI, coordinates levelSize, int maxHp) : Entity(sprite, position, size){
	this->cooldownTime = cooldownTime;
	this->cooldown = 0;
	this->enemyAI = enemyAI;
	this->lastMoveTime = 0;
	this->currentlyMoving = false;
	this->moveTarget = { 0, 0 };
	this->levelSize = levelSize;
	this->maxHp = maxHp;
	this->hp = maxHp;
}

Enemy::Enemy() : Entity(){
	this->cooldownTime = 1;
	this->cooldown = 0;
	this->currentlyMoving = false;
	this->moveTarget = { 0, 0 };
	this->enemyAI = 1;
	this->lastMoveTime = 0;
	this->levelSize = { 0, 0 };
	this->hp = 5;
	this->maxHp = 5;
}

int Enemy::getCurrentHp() const {
	return hp;
}

int Enemy::getMaxHp() const{
	return maxHp;
}

void Enemy::processAI(double deltaTime) {
	shoot(deltaTime);
	move(deltaTime);
}

void Enemy::dealDamage(){
	hp--;
}


void Enemy::generatePattern(AttackType attackType, double delay, int offset, int numberOfShots, bool explosion, shotTiming timing) {
	const float lifespawn = 2.f;
	coordinates bulletSize = { 25, 25 };
	int velocity = 500;
	coordinates startPosition = { int(position.y + size.y), int(position.x + size.x / 2 - bulletSize.x / 2) };
	double totalDelay = delay;
	vector<coordinates> angle;

	if (attackType == AttackType::Simple) { 
		for (int i = 0; i < numberOfShots; i++) {
			angle.push_back({ 1, 0 });
		}
	}
	else if (attackType == AttackType::Circular) {
		for (int i = 0; i < numberOfShots; i++) {
			int y = sin(2 * 3.1415 * (i / float(numberOfShots)) + offset) * velocity;
			int x = cos(2 * 3.1415 * (i / float(numberOfShots)) + offset) * velocity;
			angle.push_back({ y, x });
		}
	}
	else if ((attackType == AttackType::Big)) { //Du¿y
		velocity = 300;
		bulletSize = { 75, 75 };
		for (int i = 0; i < numberOfShots; i++) {
			angle.push_back({ 1, 0 });
		}
	}
	else if ((attackType == AttackType::BeamAttack)) {
		velocity = 0;
		bulletSize = { 500, 50 };
		lastMoveTime = -1;
		currentlyMoving = false;
		double delay = 1;
		Bullet bullet = Bullet(Sprite::FakeBeam, startPosition, bulletSize, { 1, 0 }, velocity, delay, Owner::System);
		bulletQueue.push_back({ bullet, 0 });
		bulletSize = { 500, 50 };
		bullet = Bullet(Sprite::Bullet, startPosition, bulletSize, { 1, 0 }, velocity, 0.2, Owner::Enemy);
		bulletQueue.push_back({ bullet, delay });
		return;
	}
	else if ((attackType == AttackType::CrossAttack)) {
		velocity = 0;
		bulletSize = { 25, levelSize.y };
		lastMoveTime = -1;
		currentlyMoving = false;
		angle.push_back({ 1, 0 });
		angle.push_back({ 1, -1 });
		angle.push_back({ 1, 1 });
		delay = 1;
		for (int i = 0; i < angle.size(); i++) {
			Bullet bullet = Bullet(Sprite::FakeBeam, {startPosition.y + 100 * (i + 1) , 0}, bulletSize, angle[i], velocity, delay, Owner::System);
			bulletQueue.push_back({ bullet, 0 });
			bullet = Bullet(Sprite::Bullet, { startPosition.y + 100 * (i + 1) , 0 }, bulletSize, angle[i], velocity, 0.3, Owner::Enemy);
			bulletQueue.push_back({ bullet, delay });
		}
		return;
	}

	for (int i = 0; i < numberOfShots; i++) {
		Bullet bullet = Bullet(Sprite::Bullet, startPosition, bulletSize, angle[i], velocity, lifespawn, Owner::Enemy);
		if (explosion) {
			for (int j = 0; j < numberOfShots; j++) {
				coordinates explosionAngle = { int(sin(2 * 3.1415 * (j / 8.f)) * velocity), int(cos(2 * 3.1415 * (j / 8.f)) * velocity) };
				bullet.addExplosion(Bullet(Sprite::Bullet, startPosition, bulletSize, explosionAngle, velocity, lifespawn, Owner::Enemy));
			}
		}
		bulletQueue.push_back({ bullet, totalDelay });
		if (timing == shotTiming::Delayed)
			totalDelay += delay;
	}

}

vector<pair<Bullet, double>>* Enemy::getBulletQueue() {
	return &bulletQueue;
}

