/*
 * Copyright (C) Victor Nivet
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

#include "sprites.h"
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <SDL.h>

class Character;

namespace mugen {

struct sffv1color_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct sffv1palette_t {
	sffv1color_t colors[PALETTE_NCOLORS];
};

struct sffv1sprite_t {
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

struct sffv1group_t {
	// map: index in group -> absolute image index
	std::unordered_map<size_t, size_t> i;
};

class Sffv1: public SpriteHandler
{
public:
	Sffv1(Character& character, const char* filename);
	~Sffv1();
	void load();
	void load(std::vector< spriteref >::iterator first, std::vector< spriteref >::iterator last);
protected:
	void loadSffFile();
	void loadSharedPalettes();
	// true if there is a palette file that was sucessfully read
	// false if not
	bool readActPalette(const char* filepath);
	sffv1palette_t getPaletteForSprite(size_t spritenumber);
	SDL_Surface * renderToSurface();
private:
	Character & character;
	std::string filename;
	size_t currentSprite;
	size_t currentPalette;
	uint32_t ngroups;
	uint32_t nimages;
	std::vector<sffv1sprite_t> sprites;
	bool sharedPalette; // if not, it's an individual palette
	std::vector<sffv1palette_t> palettes;
	std::unordered_map<size_t, sffv1group_t> groups;
};

}

#endif // SFFV1_H