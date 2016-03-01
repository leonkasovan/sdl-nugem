#ifndef SFFV2_H
#define SFFV2_H

#include "../spritehandler.h"
#include <string>
#include <vector>
#include <SDL.h>

// SFFv2 sprite format, as documented in https://web.archive.org/web/20150510210608/http://elecbyte.com/wiki/index.php/SFFv2

// This only provides support for SFF v2.00 ?
// 24bit / 32bit sprites don't work?
struct sprite_t {
	uint16_t groupno;
	uint16_t itemno;
	uint16_t width;
	uint16_t height;
	uint16_t axisx;
	uint16_t axisy;
	uint16_t indexlinked;
	uint8_t fmt; // Format: 0 -> raw, 1 -> invalid, 2 -> RLE8, 3 -> RLE5, 4 -> LZ5
	uint8_t coldepth;
	uint32_t data_offset;
	uint32_t data_length;
	uint16_t palette_index;
	uint16_t flags; // bit 0 -> if value = 0, literal (use ldata); if value = 1, translate (use tdata & decompress on load)
	// bit 1 to 15: unused
	SDL_Texture * texture;
};

struct pallet_t {
	uint16_t groupno;
	uint16_t itemno;
	uint16_t numcols; // Number of colors
	uint16_t indexlinked;
	uint32_t ldata_offset;
	uint32_t data_length;
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
	void outputColoredPixelFromPallet(uint8_t color, const uint32_t index_pixel, const pallet_t& pallet, SDL_Surface* surface, const uint32_t surface_size);
private:
	std::string filename;
	std::vector<sprite_t> sprites;
	std::vector<pallet_t> pallets;
	uint32_t currentsprite;
	uint32_t nsprites;
	uint32_t npallets;
	uint8_t * ldata;
	uint32_t ldata_length;
	uint8_t * tdata;
	uint32_t tdata_length;
	int32_t currentSprite;
	SDL_Texture * texture;
};

#endif // SFFV2_H