#include "entity.h"
#include "config.h"


void Entity::applyMoves(double deltaTime) {
	if (movementQueue.size() == 0)
		return;
	coordinates movement = { 0, 0 };
	coordinates angle = { 0, 0 };
	for (int i = 0; i < movementQueue.size(); i++) {
		coordinates direction = movementQueue[i].first;
		int speed = movementQueue[i].second;
		movement.y += direction.y * speed;
		angle.y += direction.y;
		movement.x += direction.x * speed;
		angle.x += direction.x;
	}
	double vectorLen = sqrt(angle.y * angle.y + angle.x * angle.x);
	if (vectorLen <= 0) 
		return;
	position.y += movement.y / vectorLen * deltaTime;
	position.x += movement.x / vectorLen * deltaTime;
	movementQueue.clear();
}



coordinates Entity::getSize() const {
	return size;
}

Entity::Entity(Sprite sprite, coordinates position, coordinates size) {
	this->sprite = sprite;
	this->size = size;
	this->position.x = float(position.x);
	this->position.y = float(position.y);
}

Entity::Entity(){
	sprite = Sprite(0);
	position = { 0, 0 };
	size = { 50, 50 };
}

Sprite Entity::getSprite() const {
	return sprite;
}

void Entity::queueMove(coordinates direction, int speed) {
	movementQueue.push_back({ direction, speed });
}

coordinatesF Entity::getPosition() const {
	return { position.y, position.x };
}
