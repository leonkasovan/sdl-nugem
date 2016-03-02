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

// Function for both SFFv1 and SFFv2 sprites
std::array<uint8_t, 4> extract_version(std::ifstream & fileobj);

// Little endian to big endian
uint32_t read_uint32(std::ifstream & fileobj);

uint16_t read_uint16(std::ifstream & fileobj);

#endif // SPRITEHANDLER_H