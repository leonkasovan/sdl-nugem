#ifndef SPRITEHANDLER_H
#define SPRITEHANDLER_H

#include <SDL.h>
#include <array>
#include <fstream>

class SpriteHandler {
public:
	virtual ~SpriteHandler() {};
	virtual SDL_Surface * getSurface() = 0;
	virtual const uint32_t getTotalSpriteNumber() const = 0;
	virtual void setSprite(int n) = 0;
};

#endif // SPRITEHANDLER_H