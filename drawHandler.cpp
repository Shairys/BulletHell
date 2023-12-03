#include "drawHandler.h"


DrawHandler::~DrawHandler() {
	for (int i = 0; i < int(Sprite::Size); i++) {
		SDL_Texture* sprite = getSprite(Sprite(i));
		if(sprite != NULL)
			SDL_DestroyTexture(sprite);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

DrawHandler::DrawHandler(coordinates levelSize){
	this->levelSize = levelSize;
	error = false;
	rc = SDL_CreateWindowAndRenderer(config::screenWidth, config::screenHeight, 0,
		&window, &renderer);
	viewPosition.x = levelSize.x / 2;
	viewSize.y = config::screenHeight;
	viewSize.x = config::screenWidth;
	viewPosition.y = float(levelSize.y - viewSize.y);

	currentPosition = { float(levelSize.y), 0 };
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, config::screenWidth, config::screenHeight);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "BulletHell");
	loadTextures();
	if (error)
		exit(0);

}

void DrawHandler::loadTextures() {
	SDL_Surface* surface = NULL;
	for (int i = 0; i < int(Sprite::Size); i++) {
		surface = SDL_LoadBMP(spritePath[i]);
		if (surface == NULL) {
			printf("surface error %s %s\n", spritePath[i], SDL_GetError());
			error = 1;
			SDL_FreeSurface(surface);
			return;
		}
		sprites[i] = SDL_CreateTextureFromSurface(renderer, surface);
		if (sprites[i] == NULL) {
			printf("texture error %s %s\n", spritePath[i], SDL_GetError());
			error = 1;
			SDL_FreeSurface(surface);
			return;
		}
		SDL_FreeSurface(surface);
	}
}


SDL_Texture* DrawHandler::getSprite(Sprite spriteID){
	return sprites[int(spriteID)];
}

int DrawHandler::getColor(Color colorID){
	return colors[int(colorID)];
}

void DrawHandler::clear() {
	SDL_RenderClear(renderer);
}

void DrawHandler::display() {
	SDL_RenderPresent(renderer);
}

void DrawHandler::drawTexture(Sprite spriteID, coordinatesF position){
	SDL_Texture* sprite = getSprite(spriteID);
	SDL_Rect rectPos;
	int width, height;
	SDL_QueryTexture(sprite, NULL, NULL, &width, &height);
	coordinates mappedPosition = mapToView(position);
	rectPos.x = mappedPosition.x;
	rectPos.y = mappedPosition.y;
	rectPos.w = width;
	rectPos.h = height;
	SDL_RenderCopy(renderer, sprite, NULL, &rectPos);
}

void DrawHandler::drawTexture(Sprite spriteID, coordinatesF position, coordinates size, bool map) {
	SDL_Texture* sprite = getSprite(spriteID);
	SDL_Rect rectPos;
	coordinates mappedPosition = { position.y, position.x };
	if (map)
		mappedPosition = mapToView(position);
	rectPos.x = mappedPosition.x;
	rectPos.y = mappedPosition.y;
	rectPos.w = size.x;
	rectPos.h = size.y;
	SDL_RenderCopy(renderer, sprite, NULL, &rectPos);
}

void DrawHandler::draw(Entity& entity) {
	coordinatesF entityPosition = entity.getPosition();
	drawTexture(entity.getSprite(), { entityPosition.y, entityPosition.x }, entity.getSize());
}

void DrawHandler::drawBackground(int stage){
	Sprite spriteID;
	if (stage == 1)
		spriteID = Sprite::FirstStageBackground;
	else if(stage == 2)
		spriteID = Sprite::SecondStageBackground;
	else if(stage == 3)
		spriteID = Sprite::ThirdStageBackground;
	SDL_Texture* sprite = getSprite(spriteID);
	SDL_Rect rectPosition;
	double renderedPosition = viewPosition.y;
	coordinates spriteSize = getSpriteSize(spriteID);
	int width = spriteSize.x, height = spriteSize.y;
	const double widthRatio = (double)width / double(levelSize.x);
	const double heightRatio = (double)height / double(levelSize.y);
	rectPosition.w = int(config::screenWidth * widthRatio);
	rectPosition.h = int(config::screenHeight * heightRatio);
	if (renderedPosition - config::screenHeight < 0) {
		renderedPosition += ((int(-renderedPosition) + config::screenHeight) / int(levelSize.y - config::screenHeight) + 1) * (levelSize.y - config::screenHeight);
	}
	rectPosition.x = (double)viewPosition.x * widthRatio;
	rectPosition.y = renderedPosition * heightRatio - rectPosition.h;
	SDL_RenderCopy(renderer, sprite, &rectPosition, NULL);
}

void DrawHandler::drawString(coordinates position, const char* text, coordinates size) {
	SDL_Texture* charset = getSprite(Sprite::Charset);
	int x = position.x, y = position.y;
	int offsetX, offsetY, character;
	SDL_Rect source, destination;
	source.w = config::pixelsPerChar;
	source.h = config::pixelsPerChar;
	destination.w = size.x;
	destination.h = size.y;
	while (*text) {
		character = *text & 255;
		offsetX = (character % 16) * config::pixelsPerChar;
		offsetY = (character / 16) * config::pixelsPerChar;
		source.x = offsetX;
		source.y = offsetY;
		destination.x = x;
		destination.y = y;
		SDL_RenderCopy(renderer, charset, &source, &destination);
		x += size.x;
		text++;
	};
}

void DrawHandler::scroll(double speed, double deltaTime){
	viewPosition.y += -(speed * deltaTime);
}


coordinates DrawHandler::mapToView(coordinatesF position){
	int y = position.y - viewPosition.y;
	int x = position.x - viewPosition.x;
	return { y, x };
}

coordinates DrawHandler::getViewPosition() const{
	return { int(viewPosition.y), int(viewPosition.x) };
}

coordinates DrawHandler::getViewSize() const{
	return viewSize;
}

void DrawHandler::applyHorizontalMovementToView(coordinatesF position, coordinates playerSize){
	viewPosition.x = max(0.f, position.x - config::screenWidth / 2 + playerSize.x / 2);
	viewPosition.x = min(float(levelSize.x - config::screenWidth), viewPosition.x);
}
coordinates DrawHandler::getSpriteSize(Sprite spriteID){
	SDL_Texture* sprite = getSprite(spriteID);
	int width, height;
	SDL_QueryTexture(sprite, NULL, NULL, &width, &height);
	return { height, width };
}

void DrawHandler::resetView(){
	viewPosition.x = levelSize.x / 2;
	viewSize.y = config::screenHeight;
	viewSize.x = config::screenWidth;
	viewPosition.y = float(levelSize.y - viewSize.y);
}

