#include "game.h"

Game::Game() : drawHandler(DrawHandler({ 4800, 2400 })) { 
	stage = 1;
	state = GameState::Menu;
	initialize();
} 

Game::~Game() {
	SDL_Quit();
}

void Game::addBulletsFromQueue(vector<pair<Bullet, double>>* bulletQueue){
	for (int i = 0; i < bulletQueue->size(); i++) {
		Bullet bullet = Bullet((*bulletQueue)[i].first);
		bulletsDelayQueue.push_back({ bullet, (*bulletQueue)[i].second });
	}
	bulletQueue->clear();
}

void Game::handleInput(double deltaTime){
	// obs³uga zdarzeñ (o ile jakieœ zasz³y)
	SDL_Event event;
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_RIGHT]) {
		player.queueMove({ 0, 1 }, config::playerSpeed);
	}
	if (keyboard[SDL_SCANCODE_LEFT]) {
		player.queueMove({ 0, -1 }, config::playerSpeed);
	}
	if (keyboard[SDL_SCANCODE_UP]) {
		player.queueMove({ -1, 0 }, config::playerSpeed);
	}
	if (keyboard[SDL_SCANCODE_DOWN]) {
		player.queueMove({ 1, 0 }, config::playerSpeed);
	}
	if (keyboard[SDL_SCANCODE_N]) {
		state = GameState::Restart;
	}
	if (keyboard[SDL_SCANCODE_ESCAPE]) {
		state = GameState::Close;
	}
	if (keyboard[SDL_SCANCODE_SPACE]) {
		player.shoot(deltaTime);
	}
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			state = GameState::Close;
	}
}

void Game::setTargetFramerate(const int framerate, const int timeSinceLastFrame){
	//Ustawienie docelowych FPS na targetFPS
	const int delay = 1000 / targetFPS;
	if (delay > timeSinceLastFrame)
		SDL_Delay(delay - timeSinceLastFrame);
}

void Game::scrollEntities(double deltaTime){
	//Przesuniêcie wszystkiego w górê, wraz z plansz¹
	drawHandler.display();
	drawHandler.scroll(scrollingSpeed, deltaTime);
	player.queueMove({ -1, 0 }, scrollingSpeed);
	player.applyMoves(deltaTime, drawHandler.getViewPosition(), drawHandler.getViewSize(), levelSize);
	
	enemy.queueMove({ -1, 0 }, scrollingSpeed);
 	enemy.applyMoves(deltaTime);
	for (int i = 0; i < bullets.size(); i++) {
		bullets[i].queueMove({ -1, 0 }, scrollingSpeed);
		bullets[i].applyMoves(deltaTime);
	}
	for (int i = 0; i < bulletsDelayQueue.size(); i++) {
		bulletsDelayQueue[i].first.queueMove({ -1, 0 }, scrollingSpeed);
		bulletsDelayQueue[i].first.applyMoves(deltaTime);
	}
}

void Game::handleLogic(double deltaTime){ //Obs³uga logiki oraz rysowania czêœci planszy
	drawHandler.clear();
	drawHandler.drawBackground(stage);
	if (player.getCurrentHp() == 0) {
		state = GameState::GameOver;
		return;
	}
	if (enemy.getCurrentHp() == 0) {
		state = GameState::Victory;
		return;
	}

	//Rysowanie jest powi¹zane mocno z logik¹, wiêc jest tutaj wykonywane
	drawUI();
	enemy.processAI(deltaTime);
	addBulletsFromQueue(enemy.getBulletQueue());
	player.applyMoves(deltaTime, drawHandler.getViewPosition(), drawHandler.getViewSize(), levelSize);
	drawHandler.applyHorizontalMovementToView(player.getPosition(), player.getSize());

	drawHandler.draw(player);
	drawHandler.draw(enemy);
	addBulletsFromQueue(player.getBulletQueue());
	addBulletsFromQueue(enemy.getBulletQueue());
	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i].calculateBullet(deltaTime)) {
			addBulletsFromQueue(bullets[i].getBulletQueue());
			bullets.swap(i, bullets.size() - 1);
			bullets.pop_back();
			i--;
		}
	}
	processBulletQueue(deltaTime);
	for (int i = 0; i < bullets.size(); i++) {
		drawHandler.draw(bullets[i]);
	}
	player.recalculateInvicibilityCooldown(deltaTime);

}

void Game::processBulletQueue(double deltaTime){ //Pociski czekaj¹ w kolejce a¿ czas minie i zostan¹ wystrzelone
	for (int i = 0; i < bulletsDelayQueue.size(); i++) {
		bulletsDelayQueue[i].second -= deltaTime;
		if (bulletsDelayQueue[i].second < 0) {
			bullets.push_back(bulletsDelayQueue[i].first);
			bulletsDelayQueue.swap(i, bulletsDelayQueue.size() - 1);
			bulletsDelayQueue.pop_back();
			i--;
		}
	}
}

void Game::processHitBoxes(){
	coordinates enemyPosition = { int(enemy.getPosition().y), int(enemy.getPosition().x) };
	coordinates enemySize = enemy.getSize();
	coordinates playerPosition = { int(player.getPosition().y), int(player.getPosition().x) };
	coordinates playerSize = player.getSize();
	for (int i = 0; i < bullets.size(); i++) {
		Bullet* bullet = &bullets[i];
		coordinates bulletPosition = {int( bullet->getPosition().y), int(bullet->getPosition().x) };
		coordinates bulletSize = bullet->getSize();
		if (bullet->getOwner() == Owner::Enemy) {
			if (checkCollision(playerPosition, playerSize, bulletPosition, bulletSize)) {
				if (player.dealDamage()) {
					bullet->setLifespawn(0);
					enemyScore++;
				}
			}
		}
		if (bullet->getOwner() == Owner::Player) {
			if (checkCollision(enemyPosition, enemySize, bulletPosition, bulletSize)) {
				bullet->setLifespawn(0);
				player.addScore();
				enemy.dealDamage();
			}
		}
		if (bullet->getOwner() == Owner::Medic) { //Bonus, czyli apteczka, jest traktowana jako statyczny pocisk
			if (checkCollision(playerPosition, playerSize, bulletPosition, bulletSize)) {
				player.heal();
				player.addScore();
				bullet->setLifespawn(0);
			}
		}
	}
}

void Game::drawUI(){
	char text[128];
	sprintf(text, "Czas trwania = %.1lf s", worldTime);
	drawHandler.drawString({ 0, 0 }, text);
	sprintf(text, "Wynik gracza: %d\n", player.getScore());
	Sprite scoreSprite;
	int score = player.getScore();
	if (score < 30)
		scoreSprite = Sprite::F_Score;
	else if (score < 80)
		scoreSprite = Sprite::D_Score;
	else if(score < 140)
		scoreSprite = Sprite::E_Score;
	else if (score < 180)
		scoreSprite = Sprite::C_Score;
	else if (score < 240)
		scoreSprite = Sprite::B_Score;
	else if (score < 450)
		scoreSprite = Sprite::A_Score;
	else
		scoreSprite = Sprite::A_Score;
	drawHandler.drawTexture(scoreSprite, { 50, 25 }, { 75, 75 }, false);
	drawHandler.drawString({ 0, 150 }, text);
	drawHandler.drawTexture(Sprite::HealthBar, { 20, 10 }, { 25, 200 * player.getCurrentHp() / player.getMaxHp() }, false);
	drawHandler.drawTexture(Sprite::EnemyHealthBar, { 20, config::screenWidth - 250 }, { 25, 200 * enemy.getCurrentHp() / enemy.getMaxHp() }, false);
}

void Game::drawMenu(){
	drawHandler.clear();
	char text[128];
	sprintf(text, "Stage 1");
	drawHandler.drawString({ 100, 50 }, text, { 32, 32 });
	sprintf(text, "Stage 2");
	drawHandler.drawString({ 200, 50 }, text, { 32, 32 });
	sprintf(text, "Stage 3");
	drawHandler.drawString({ 300, 50 }, text, { 32, 32 });
	sprintf(text, "Exit");
	drawHandler.drawString({ 400, 50 }, text, { 32, 32 });
	drawHandler.display();
}

void Game::handleMenu(){
	initialized = false;
	SDL_Event event;
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_ESCAPE]) {
		state = GameState::Close;
	}
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			state = GameState::Close;
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (checkCollision({ 100, 50 }, { 32, 32 * 7 }, { event.button.y, event.button.x }, { 2, 2 })) {
				setStage(1); // Stage 1
				state = GameState::Start;
			}
			if (checkCollision({ 200, 50 }, { 32, 32 * 7 }, { event.button.y, event.button.x }, { 2, 2 })) {
				setStage(2); // Stage 2
				state = GameState::Start;
			}
			if (checkCollision({ 300, 50 }, { 32, 32 * 7 }, { event.button.y, event.button.x }, { 2, 2 })) {
				setStage(3); // Stage 3
				state = GameState::Start;
			}
			if (checkCollision({ 400, 50 }, { 32, 32 * 7 }, { event.button.y, event.button.x }, { 2, 2 })) {
				//exit
				state = GameState::Close;
			}
		}
	}
}

void Game::setEnemy(int stage){
	levelSize = { 4800, 2400 };
	coordinates playerSize = { 25, 25 };
	coordinates playerPosition = { levelSize.y - playerSize.y - 100, levelSize.x / 2 + playerSize.y };
	int enemyAI = stage;
	Sprite enemySprite;
	if (stage == 1)
		enemySprite = Sprite::FirstEnemy;
	else if(stage == 2)
		enemySprite = Sprite::SecondEnemy;
	else
		enemySprite = Sprite::ThirdEnemy;
	enemy = Enemy(enemySprite, { levelSize.y - config::screenHeight + 100, playerPosition.x }, { 75, 75 }, 0.5f, enemyAI, levelSize, 15);
}

void Game::setStage(int stage){
	this->stage = stage;
	setEnemy(stage);
}

void Game::displayGameOver(){
	drawHandler.clear();
	char text[128];
	sprintf(text, "GAME OVER!");
	drawHandler.drawString({ 100, 100 }, text, { 32, 32 });
	sprintf(text, "Retry");
	drawHandler.drawString({ 300, 100 }, text, { 32, 32 });
	sprintf(text, "Menu");
	drawHandler.drawString({ 400, 100 }, text, { 32, 32 });
	drawHandler.display();
}

void Game::handleGameOver(){
	initialized = false;
	SDL_Event event;
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_ESCAPE]) {
		state = GameState::Close;
	}
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			state = GameState::Close;
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (checkCollision({ 300, 100 }, { 32, 32 * 5 }, { event.button.y, event.button.x }, { 2, 2 })) {
				state = GameState::Start;
			}
			if (checkCollision({ 400, 100 }, { 32, 32 * 4 }, { event.button.y, event.button.x }, { 2, 2 })) {
				state = GameState::Menu;
			}
		}
	}
}

void Game::initialize(){
	const int paddingBottom = config::paddingBottom;
	const double cooldownTime = 0.5;
	const double invincibilityTime = 1;
	const int maxHp = 5;
	this->levelSize = levelSize;
	coordinates playerSize = { config::playerSize, config::playerSize };
	coordinates playerPosition = { levelSize.y - playerSize.y - paddingBottom, levelSize.x / 2 + playerSize.y };
	player = Player(Sprite::Player, playerPosition, playerSize, cooldownTime, invincibilityTime, maxHp);
	worldTime = 0;
	playerScore = 0, enemyScore = 0;
	lastFrame = 0;
	setStage(stage);
	initialized = true;
	drawHandler.resetView();
}

void Game::displayVictory(){
	drawHandler.clear();
	char text[128];
	sprintf(text, "You won!");
	drawHandler.drawString({ 100, 100 }, text, { 32, 32 });
	if (stage < 3) {
		sprintf(text, "Next Stage!");
		drawHandler.drawString({ 300, 100 }, text, { 32, 32 });
	}
	drawHandler.display();
}

void Game::handleVictory(){
	initialized = false;
	SDL_Event event;
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	if (keyboard[SDL_SCANCODE_ESCAPE]) {
		state = GameState::Close;
	}
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			state = GameState::Close;
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (stage < 3 && checkCollision({ 300, 100 }, { 32, 32 * 11 }, { event.button.y, event.button.x }, { 2, 2 })) {
				stage++;
				state = GameState::Start;
			}
		}
	}
}

void Game::handlePickups(double deltaTime){
	pickupTimer -= deltaTime;
	if (pickupTimer > 0)
		return;
	pickupTimer = 0.5; // Czas pojawienia siê bonusów
	int x = rand() % levelSize.x;
	int y = rand() % config::screenHeight / 2 - config::screenHeight / 4;
	bullets.push_back(Bullet(Sprite::Health, { x, int(player.getPosition().y) + y }, { 50, 50 }, { 0, 0 }, 0, 10, Owner::Medic));
}

void Game::displayHighscore(){
	vector < pair<int, const char*>> highscores;
}

void Game::handleHighscore(){
	
}

int Game::start() {
	double deltaTime;
	lastFrame = SDL_GetTicks();
	int frameStart;

	while (state != GameState::Close) {
		frameStart = SDL_GetTicks();
		if (state == GameState::Menu) {
			drawMenu();
			handleMenu();
		}
		else if (state == GameState::Start) {
			frameStart = SDL_GetTicks();
			deltaTime = double(frameStart - lastFrame) * 0.001;
			if (deltaTime > 0.5)
				deltaTime = 0; //Zapobiega nag³ym skokom przy np. debugowaniu
			lastFrame = frameStart;
			worldTime += deltaTime;
			if (!initialized)
				initialize();
			handleLogic(deltaTime);
			handlePickups(deltaTime);
			handleInput(deltaTime);
			player.applyMoves(deltaTime, drawHandler.getViewPosition(), drawHandler.getViewSize(), levelSize);
			
			scrollEntities(deltaTime);
			processHitBoxes();
		}
		else if (state == GameState::Restart) {
			return 1; // Zwraca true, w pêtli jest gra jest tworzona po raz kolejny
		}
		else if (state == GameState::GameOver) {
			displayGameOver();
			handleGameOver();
		}
		else if (state == GameState::Victory) {
			displayVictory();
			handleVictory();
		}
		else if (state == GameState::Highscore) {
			displayHighscore();
			handleHighscore();
		}
		setTargetFramerate(targetFPS, SDL_GetTicks() - frameStart);
	}
	return 0;

}

