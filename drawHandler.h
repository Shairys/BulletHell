#pragma once
#include "./SDL2-2.0.10/include/SDL.h"
#include "./SDL2-2.0.10/include/SDL_main.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "player.h"
#include "sprites.h"
#include "config.h"
#include "bullet.h"

class DrawHandler {
	int rc;
	bool error;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
	int colors[int(Color::Size)];
	SDL_Texture* sprites[int(Sprite::Size)];
	SDL_Surface* screen;
	SDL_Texture* getSprite(Sprite spriteID);
	int getColor(Color colorID);
	void loadTextures();
	coordinatesF currentPosition;
	coordinatesF viewPosition;
	coordinates viewSize;
	coordinates levelSize;

public:
	DrawHandler(coordinates levelSize);
	~DrawHandler();
	void clear();
	void display();
	void drawTexture(Sprite spriteID, coordinatesF position );
	void drawTexture(Sprite spriteID, coordinatesF position, coordinates size, bool map = true);
	void draw(Entity& entity);
	void drawBackground(int stage);
	void scroll(double speed, double deltaTime);
	void drawString(coordinates position, const char* text, coordinates size = { 8, 8 });
	coordinates mapToView(coordinatesF position);
	coordinates getViewPosition() const;
	coordinates getViewSize() const;
	void applyHorizontalMovementToView(coordinatesF position, coordinates playerSize);
	coordinates getSpriteSize(Sprite spriteID);
	void resetView();
};