#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "player.h"
#include "enemy.h"
#include "./SDL2-2.0.10/include/SDL.h"
#include "./SDL2-2.0.10/include/SDL_main.h"
#include "drawHandler.h"
#include "string.h"

const int targetFPS = 60;
const int scrollingSpeed = 600;
const coordinates levelSize = { 4800, 2400 };

class Game {
	DrawHandler drawHandler;
	Player player;
	vector<pair<Bullet, double>> bulletsDelayQueue;
	vector<Bullet> bullets;
	Enemy enemy;
	coordinates levelSize;
	GameState state;
	double worldTime;
	int lastFrame;
	int playerScore, enemyScore;
	bool initialized;
	int stage;
	double pickupTimer;

	void addBulletsFromQueue(vector<pair<Bullet, double>>* bulletQueue);
	void handleInput(double deltaTime);
	void setTargetFramerate(const int framerate, const int timeSinceLastFrame);
	void scrollEntities(double deltaTime);
	void handleLogic(double deltaTime);
	void processBulletQueue(double deltaTime);
	void processHitBoxes();
	void drawUI();
	void drawMenu();
	void handleMenu();
	void setEnemy(int stage);
	void setStage(int stage);
	void displayGameOver();
	void handleGameOver();
	void initialize();
	void displayVictory();
	void handleVictory();
	void handlePickups(double deltaTime);
	void displayHighscore();
	void handleHighscore();
	
public:
	int start();
	Game();
	~Game();
};