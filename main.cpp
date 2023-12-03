#define _USE_MATH_DEFINES
#include "game.h"

int main(int argc, char **argv) {
	srand(time(NULL));
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}
	Game* myGame = new Game;
	while (myGame->start()) {
		delete myGame;
		myGame = new Game;
	}
	delete myGame;
	
	return 0;
}
