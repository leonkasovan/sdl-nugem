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

#include "sffv1.h"

#include "../character.h"

#define ACT_PALETTES_NUMBER 256
#define READBUF_SIZE 12

#include <ios>
#include <fstream>
#include <iostream>
#include <array>
#include <SDL_image.h>


Sffv1::Sffv1(Character & chara, const char * filename): character(chara)
{
	currentSprite = 0;
	currentPalette = 0;
	uint32_t fileptr;
	uint8_t * readbuf[READBUF_SIZE];
	std::ifstream charfile(filename);
	// Reading the filename

	// First 512 bytes: header
	// Signature at the start of the file: 'ElecbyteSpr\0'
	charfile.read((char *) readbuf, 12);
	if (strcmp((char *) readbuf, "ElecbyteSpr")) {
		throw std::runtime_error(std::string("Invalid sprite file: ") + filename);
	}
	// Version bytes
	std::array<uint8_t, 4> version = extract_version(charfile);
	// if the version is too high, throw it
	if (version[3] > 1)
		throw version;
	// Number of groups
	ngroups = read_uint32(charfile);
	nimages = read_uint32(charfile);
	uint32_t nextSubfileOffset = read_uint32(charfile);
	uint32_t subheaderSize = read_uint32(charfile);
	uint8_t sharedPaletteByte;
	charfile >> sharedPaletteByte;
	sharedPalette = (sharedPaletteByte != 0);
	// Reading the subfiles
	while (charfile.good() && ((int) nextSubfileOffset) > 0 && sprites.size() < nimages) {
		sffv1sprite_t sprite;
		charfile.seekg(nextSubfileOffset, std::ios::beg);
		nextSubfileOffset = read_uint32(charfile);
		sprite.dataSize = read_uint32(charfile);
		sprite.axisX = read_uint16(charfile);
		sprite.axisY = read_uint16(charfile);
		sprite.group = read_uint16(charfile);
		sprite.groupimage = read_uint16(charfile);
		sprite.linkedindex = read_uint16(charfile);
		uint8_t samePaletteAsPreviousByte;
		charfile >> samePaletteAsPreviousByte;
		sprite.samePaletteAsPrevious = (samePaletteAsPreviousByte != 0);
		sprite.hasOwnPalette = false;
		charfile.seekg(13, std::ios::cur); // skipping the next 12 bytes, from byte 19 to byte 31
		// According to formats.txt:
		// "PCX graphic data. If palette data is available, it is the last 768 bytes."
		sprite.data = new uint8_t[sprite.dataSize];
		charfile.read((char *) sprite.data, sprite.dataSize);
		
		if (sprite.dataSize > 768 && sprite.data[sprite.dataSize - 1 - 768] == 0x0C)
			sprite.hasOwnPalette = true;
		
		sprites.push_back(sprite);
	}
	charfile.close();
	// Now we have to read the palette files
	bool continueLoop = true;
	// We get the values of pal1, pal2, pal3, ... etc in order until pal12
	for (int i = 1; continueLoop && i <= 12; i++) {
		std::string keyname = "pal";
		keyname += std::to_string(i);
		try {
			std::string actfilename = character.getdir() + "/" + (std::string) character.getdef().at("Files").at(keyname);
			continueLoop = readActPalette(actfilename.c_str());
		}
		catch
			(std::out_of_range) {
			break;
		}
	}
}

Sffv1::~Sffv1()
{
	for (int i = 0; i < sprites.size(); i++) {
		delete [] sprites[i].data;
	}
}

const size_t Sffv1::getTotalSpriteNumber() const
{
	return sprites.size();
}

void Sffv1::setSprite(size_t n)
{
	currentSprite = n;
}

const size_t Sffv1::getTotalPaletteNumber() const
{
	return palettes.size();
}

void Sffv1::setPalette(size_t n)
{
	currentPalette = n;
}

sffv1palette_t Sffv1::getPaletteForSprite(size_t spritenumber)
{
	sffv1palette_t s;
	long paletteSpriteNumber = spritenumber;
	size_t iterationNumber = sprites.size();
	
	while (sprites[paletteSpriteNumber].samePaletteAsPrevious && iterationNumber > 0) {
		iterationNumber--;
		paletteSpriteNumber--;
		if (paletteSpriteNumber < 0)
			paletteSpriteNumber += sprites.size();
	}
	sffv1sprite_t & paletteSprite = sprites[paletteSpriteNumber];
	if (paletteSprite.dataSize > 768 && paletteSprite.data[paletteSprite.dataSize - 768 - 1] == 0x0C) {
		uint8_t * paletteData = paletteSprite.data + (paletteSprite.dataSize - 768);
		for (int i = 0; i < 256; i++) {
			s.colors[i] = (sffv1color_t) { *(paletteData + 3 * i), *(paletteData + 3 * i + 1), *(paletteData + 3 * i + 2) };
		}
		return s;
	}
	return palettes[currentPalette];
}

SDL_Surface * Sffv1::getSurface()
{
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	size_t displayedSpriteNumber = currentSprite;
	if (sprites[currentSprite].linkedindex && !sprites[currentSprite].dataSize)
		displayedSpriteNumber = sprites[currentSprite].linkedindex;
	sffv1sprite_t & displayedSprite = sprites[displayedSpriteNumber];
	
// 	if the sprite is supposed to use the same palette as the previous sprite, then we get this palette
	SDL_Surface * surface = nullptr;
	
	// decoding the PCX
	uint16_t Xmin = *( (uint16_t * ) displayedSprite.data + 2);
	uint16_t Ymin = *( (uint16_t * ) displayedSprite.data + 3);
	uint16_t Xmax = *( (uint16_t * ) displayedSprite.data + 4);
	uint16_t Ymax = *( (uint16_t * ) displayedSprite.data + 5);
	uint8_t nplanes = *( (uint8_t * ) displayedSprite.data + 65);
	uint16_t bytesPerLine = *( (uint16_t * ) displayedSprite.data + 33);
	uint16_t width = Xmax - Xmin + 1;
	uint16_t height = Ymax - Ymin + 1;
	uint32_t totalBytesPerLine = nplanes * bytesPerLine;
	surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
	SDL_LockSurface(surface);
	// find the right palette
	sffv1palette_t palette = getPaletteForSprite(currentSprite);
	uint8_t * dataStart = displayedSprite.data + 128;
	uint32_t * pixels = (uint32_t * ) surface->pixels;
	size_t i_pixel, i_byte;
	for (i_pixel = 0, i_byte = 0; i_pixel < width * height && i_byte < (displayedSprite.dataSize - 128); i_byte++) {
		uint16_t runLength;
		SDL_Color sdlcolor;
		uint8_t colorIndex;
		if ((dataStart[i_byte] & 0xC0) == 0xC0) { // RLE byte
			runLength = dataStart[i_byte] & 0x3F;
			i_byte++;
			colorIndex = dataStart[i_byte];
		}
		else { // simple pixel byte
			runLength = 1;
			colorIndex = dataStart[i_byte];
		}
		if (colorIndex) {
				sffv1color_t & color = palette.colors[dataStart[i_byte]];
				sdlcolor = (SDL_Color) { color.red, color.green, color.blue, 0xFF };
			}
			else
				sdlcolor = (SDL_Color) { 0, 0, 0, 0 };
		for (int runCount = runLength; runCount > 0; runCount--, i_pixel++)
			*(pixels + i_pixel) = SDL_MapRGBA(surface->format, sdlcolor.r, sdlcolor.g, sdlcolor.b, sdlcolor.a);
	}
	SDL_UnlockSurface(surface);
	
	// use SDL_image to read the PCX data
	
	
// 	SDL_RWops * imgdata = SDL_RWFromConstMem(sprite.data, sprite.dataSize);
// 	SDL_Surface * surface = IMG_LoadPCX_RW(imgdata);
// 	SDL_RWclose(imgdata);
// 	// TODO deal with transparency
// 	if (!sprite.dataPalette) {
// 		// if it reuses the palette from some previous sprite
// 		size_t spriteWithPalette = currentSprite;
// 		size_t loopLimit = 0;
// 		for (spriteWithPalette = currentSprite; spriteWithPalette >= 0 && sprites[spriteWithPalette].samePaletteAsPrevious && !sprites[spriteWithPalette].dataPalette && loopLimit < sprites.size(); loopLimit++)
// 			spriteWithPalette = (spriteWithPalette - 1 + sprites.size()) % sprites.size();
// 		if (spriteWithPalette >= 0 && sprites[spriteWithPalette].dataPalette && sprites[spriteWithPalette + 1].samePaletteAsPrevious) {
// 			// copy palette from previous sprite
// 			// first copy the colors
// 			sffv1palette_t palette;
// 			uint8_t * paletteData = sprites[spriteWithPalette].data + (sprites[spriteWithPalette].dataSize - 768);
// 			for (int i = 0; i < 256; i++) {
// 				palette.colors[i] = (sffv1color_t) { *(paletteData + 3 * i), *(paletteData + 3 * i + 1), *(paletteData + 3 * i + 2) };
// 			}
// 			SDL_Surface * pcx_surface = surface;
// 			surface = SDL_CreateRGBSurface(0, pcx_surface->w, pcx_surface->h, 32, rmask, gmask, bmask, amask);
// 			SDL_FreeSurface(pcx_surface);
// 		}
// 	}
	return surface;
}

bool Sffv1::readActPalette(const char * filepath)
{
	sffv1palette_t palette;
	std::ifstream actfile;
	try {
		actfile.open(filepath);
		if (actfile.fail())
			return false;
		for (int i_palette = 0; i_palette < ACT_PALETTES_NUMBER && actfile.good(); i_palette++) {
			palette.colors[i_palette] = { actfile.get(), actfile.get(), actfile.get() };
		}
		actfile.close();
	}
	catch
		(std::ios_base::failure()) {
		if (actfile.is_open())
			actfile.close();
		return false;
	}
	palettes.push_back(palette);
	return true;
}
