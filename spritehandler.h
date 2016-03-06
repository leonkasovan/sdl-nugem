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

#ifndef SPRITEHANDLER_H
#define SPRITEHANDLER_H

#include <SDL.h>
#include <array>
#include <fstream>

class SpriteHandler {
public:
	virtual ~SpriteHandler() {};
	virtual SDL_Surface * getSurface() = 0;
	virtual const size_t getTotalSpriteNumber() const = 0;
	virtual const size_t getTotalPaletteNumber() const = 0;
	virtual void setSprite(size_t n) = 0;
	virtual void setPalette(size_t n) = 0;
};

// Function for both SFFv1 and SFFv2 sprites
std::array<uint8_t, 4> extract_version(std::ifstream & fileobj);

// Little endian to big endian
uint32_t read_uint32(std::ifstream & fileobj);

uint16_t read_uint16(std::ifstream & fileobj);

#endif // SPRITEHANDLER_H