/*
 * Copyright (c) 2016 Victor Nivet
 *
 * This file is part of Nugem.
 *
 * Nugem is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * Nugem is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with Nugem.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SFFV2_H
#define SFFV2_H

#include "sprites.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>

namespace Nugem {
namespace Mugen {

/**
 * SFFv2 sprite format, as documented in https://web.archive.org/web/20150510210608/http://elecbyte.com/wiki/index.php/SFFv2
*/

class Sffv2: public SpriteHandler {
private:
	// This only provides support for SFF v2.00
	// 24bit / 32bit sprites may or may not work?
	struct SpriteInfo {
		uint16_t groupno;
		uint16_t itemno;
		uint16_t width;
		uint16_t height;
		uint16_t axisx;
		uint16_t axisy;
		uint16_t linkedindex;
		uint8_t fmt; // Format: 0 -> raw, 1 -> invalid, 2 -> RLE8, 3 -> RLE5, 4 -> LZ5
		uint8_t coldepth;
		uint32_t dataOffset;
		uint32_t dataLength;
		uint16_t paletteIndex;
		uint16_t flags; // flags w/ load information
		// bit 0 -> if value = 0, literal (use ldata); if value = 1, translate (use tdata & decompress on load)
		// bit 1 to 15: unused
		SDL_Texture * texture;
		bool usesTData() const;
	};

	struct PaletteInfo {
		uint16_t groupno;
		uint16_t itemno;
		uint16_t numcols; // Number of colors
		uint16_t linkedindex;
		uint32_t ldataOffset;
		uint32_t dataLength;
		// there are 4 bytes per color: 3 for RGB 8-bit values, and a last, unused byte
	};

	struct GroupInfo {
		std::unordered_map<size_t, size_t> i;
	};
public:
    Sffv2(const char* filename);
    ~Sffv2();
    void load();
    void load(std::vector<Spriteref>::iterator first, std::vector<Spriteref>::iterator last);
protected:
    void loadSffFile();
    SpriteInfo readSprite(std::ifstream & fileobj);
    PaletteInfo readPalette(std::ifstream & fileobj);
    SDL_Surface * renderToSurface(size_t spriteNumber, size_t currentPaletteId);
private:
	class Drawer: public SurfaceDrawer {
	public:
		Drawer(const SpriteInfo& sprite, const PaletteInfo& palette, uint8_t * ldata, uint8_t * tdata);
		~Drawer() {};
	protected:
		void draw(uint32_t * pixelData, size_t width, size_t height);
	private:
		const SpriteInfo& m_sprite;
		const PaletteInfo& m_palette;
		const uint8_t * m_ldata;
		const uint8_t * m_tdata;
	};
    static const size_t READBUF_SIZE = 32;
    std::string m_filename;
    std::vector<SpriteInfo> m_sffv2Container;
    std::vector<PaletteInfo> m_palettes;
    std::unordered_map<size_t, GroupInfo> m_groups;
    uint8_t * m_ldata;
    uint32_t m_ldataLength;
    uint8_t * m_tdata;
    uint32_t m_tdataLength;
    SDL_Texture * m_texture;
	std::vector<std::unordered_map<Spriteref, Sprite>> m_sprites;
public:
	std::vector<std::unordered_map<Spriteref, Sprite>> sprites() { return m_sprites; };
};

}
}

#endif // SFFV2_H
