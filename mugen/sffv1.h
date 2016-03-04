#ifndef SFFV1_H
#define SFFV1_H

#include "../spritehandler.h"
#include <string>
#include <vector>
#include <SDL.h>

class Character;

struct sffv1sprite_t {
	// image coordinates
	uint16_t axisX;
	uint16_t axisY;
	uint16_t group; // group number
	uint16_t groupimage; // image number (in the group)
	uint32_t dataSize;
	uint16_t previousCopyIndex; // only for a linked sprite
	bool samePaletteAsPrevious;
	uint8_t * data;
	uint8_t * paletteData;
};

class Sffv1: public SpriteHandler
{
public:
	Sffv1(const char* filename);
	~Sffv1();
	SDL_Surface * getSurface();
	const uint32_t getTotalSpriteNumber() const;
	void setSprite(int n);
protected:
	
private:
	uint32_t currentSprite;
	uint32_t ngroups;
	uint32_t nimages;
	std::vector<sffv1sprite_t> sprites;
	bool sharedPalette; // if not, it's an individual palette
};

#endif // SFFV1_H