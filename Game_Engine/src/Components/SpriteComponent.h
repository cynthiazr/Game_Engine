#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <glm/glm.hpp>
#include <SDL.h>

struct SpriteComponent {
	std::string assetID;
	int width;
	int height;
	int zIndex;
	SDL_Rect srcRect;

	SpriteComponent(std::string assetID = "", int width = 0, int height = 0, int zIndex = 0, int srcRectX = 0, int srcRectY = 0) {
		this->assetID = assetID;
		this->width = width;
		this->height = height;
		this->zIndex = zIndex;
		this->srcRect = { srcRectX, srcRectY, width, height };
	}
};

#endif // !SPRITECOMPONENT_H