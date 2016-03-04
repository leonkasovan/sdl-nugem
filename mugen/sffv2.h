#ifndef SFFV2_H
#define SFFV2_H

#include "../spritehandler.h"
#include <string>
#include <vector>
#include <SDL.h>

// SFFv2 sprite format, as documented in https://web.archive.org/web/20150510210608/http://elecbyte.com/wiki/index.php/SFFv2

// This only provides support for SFF v2.00 ?
// 24bit / 32bit sprites don't work?
struct sffv2sprite_t {
	uint16_t groupno;
	uint16_t itemno;
	uint16_t width;
	uint16_t height;
	uint16_t axisx;
	uint16_t axisy;
	uint16_t indexlinked;
	uint8_t fmt; // Format: 0 -> raw, 1 -> invalid, 2 -> RLE8, 3 -> RLE5, 4 -> LZ5
	uint8_t coldepth;
	uint32_t dataOffset;
	uint32_t dataLength;
	uint16_t paletteIndex;
	uint16_t flags; // flags w/ load information
	// bit 0 -> if value = 0, literal (use ldata); if value = 1, translate (use tdata & decompress on load)
	// bit 1 to 15: unused
	SDL_Texture * texture;
};

struct sffv2palette_t {
	uint16_t groupno;
	uint16_t itemno;
	uint16_t numcols; // Number of colors
	uint16_t indexlinked;
	uint32_t ldataOffset;
	uint32_t dataLength;
	// there are 4 bytes per color: 3 for RGB 8-bit values, and a last, unused byte
};

class Sffv2: public SpriteHandler {
public:
	Sffv2(const char* filename);
	~Sffv2();
	const uint32_t getTotalSpriteNumber() const;
	void setSprite(int n);
	SDL_Surface * getSurface();
protected:
	void outputColoredPixel(uint8_t color, const uint32_t indexPixel, const sffv2palette_t& palette, SDL_Surface* surface, const uint32_t surfaceSize);
	void readFile(const char* filename);
private:
	std::string filename;
	std::vector<sffv2sprite_t> sprites;
	std::vector<sffv2palette_t> palettes;
	uint32_t currentSprite;
	uint32_t nsprites;
	uint32_t npalettes;
	uint8_t * ldata;
	uint32_t ldataLength;
	uint8_t * tdata;
	uint32_t tdataLength;
	SDL_Texture * texture;
};

#endif // SFFV2_H