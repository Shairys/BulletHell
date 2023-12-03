#pragma once

#include "utils.h"
#include "vector.h"
#include <math.h> 
#include <iostream>
#include <stdio.h>
#include "sprites.h"

class Entity {
protected:
	Sprite sprite;
	coordinatesF position;
	vector<pair<coordinates, int>> movementQueue;
	coordinates size;

public:
	Entity(Sprite sprite, coordinates position, coordinates size);
	Entity();
	Sprite getSprite() const;
	coordinatesF getPosition() const;
	void queueMove(coordinates direction, int speed);
	void applyMoves(double deltaTime);
	coordinates getSize() const;
	
};