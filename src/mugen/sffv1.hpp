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

#ifndef SFFV1_H
#define SFFV1_H

#define PALETTE_NCOLORS 256

#include "sprites.hpp"
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <SDL.h>

namespace Nugem {
namespace Mugen {

struct Sffv1Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct Sffv1Palette {
	Sffv1Color colors[PALETTE_NCOLORS];
};

struct Sffv1Sprite {
	// image coordinates
	uint16_t axisX;
	uint16_t axisY;
	uint16_t group; // group number
	uint16_t groupimage; // image number (in the group)
	uint32_t dataSize;
	uint16_t linkedindex; // only for a linked sprite
	bool usesSharedPalette; // if the image owns its palette, or if it uses a shared palette
	uint8_t * data;
};

struct Sffv1Group {
	// map: index in group -> absolute image index
	std::unordered_map<size_t, size_t> i;
};

class Sffv1: public SpriteHandler
{
public:
	Sffv1(const char* filename, const char* paletteFile = "");
	~Sffv1();
	void load();
	void load(std::vector< Spriteref >::iterator first, std::vector< Spriteref >::iterator last);
protected:
	void loadSffFile();
	void loadSharedPalettes();
	// true if there is a palette file that was sucessfully read
	// false if not
	bool readActPalette(const char* filepath);
	   Sffv1Palette getPaletteForSprite(size_t spritenumber);
	SDL_Surface * renderToSurface();
private:
	static const size_t READBUF_SIZE = 12;
	std::string m_filename;
	std::string m_paletteFile;
	size_t m_currentSprite;
	size_t m_currentPalette;
	uint32_t m_ngroups;
	uint32_t m_nimages;
	std::vector<Sffv1Sprite> m_sffv1Container;
	bool m_sharedPalette; // if not, it's an individual palette
	std::vector<Sffv1Palette> m_palettes;
	std::unordered_map<size_t, Sffv1Group> m_groups;
	std::vector<std::unordered_map<Spriteref, Sprite>> m_sprites;
public:
	std::vector<std::unordered_map<Spriteref, Sprite>> sprites() { return m_sprites; };
};

}
}

#endif // SFFV1_H
