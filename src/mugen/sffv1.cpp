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

#define READBUF_SIZE 12

#include <ios>
#include <fstream>
#include <iostream>
#include <array>


mugen::Sffv1::Sffv1(Character & character, const char * filename): character(character), filename(filename)
{
	currentSprite = 0;
	currentPalette = 0;
	sprites.clear();
	palettes.clear();
	loadSffFile();
	loadSharedPalettes();
}

mugen::Sffv1::~Sffv1()
{
	for (int i = 0; i < sprites.size(); i++) {
		delete [] sprites[i].data;
	}
}

void mugen::Sffv1::loadSffFile()
{
	uint32_t fileptr;
	uint8_t * readbuf[READBUF_SIZE];
	std::ifstream charfile(filename.c_str());
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
		uint8_t usesSharedPaletteByte;
		charfile >> usesSharedPaletteByte;
		sprite.usesSharedPalette = (usesSharedPaletteByte != 0);
		charfile.seekg(13, std::ios::cur); // skipping the next 12 bytes, from byte 19 to byte 31
		// According to formats.txt:
		// "PCX graphic data. If palette data is available, it is the last 768 bytes."
		sprite.data = new uint8_t[sprite.dataSize];
		charfile.read((char *) sprite.data, sprite.dataSize);
		// Add the sprites' index to its group and image number
		groups[sprite.group].i[sprite.groupimage] = sprites.size();
		sprites.push_back(sprite);
	}
	charfile.close();
}

void mugen::Sffv1::loadSharedPalettes()
{
	bool continueLoop = true;
	// We get the values of pal1, pal2, pal3, ... etc in order until pal12
	for (int i = 1; continueLoop && i <= 12; i++) {
		std::string keyname = "pal";
		keyname += std::to_string(i);
		try {
			std::string actfilename = character.getdir() + "/" + (std::string) character.getdef()["Files"][keyname];
			continueLoop = readActPalette(actfilename.c_str());
		}
		catch
			(std::out_of_range) { // thrown by character.getdef() if the method at doesn't find the key
			break;
		}
	}
}

mugen::sffv1palette_t mugen::Sffv1::getPaletteForSprite(size_t spritenumber)
{
	sffv1palette_t s;
	long paletteSpriteNumber = spritenumber;
	size_t iterationNumber = sprites.size();
	if (sharedPalette && sprites[paletteSpriteNumber].usesSharedPalette)
		return palettes[currentPalette];
	while (sprites[paletteSpriteNumber].usesSharedPalette && iterationNumber > 0) {
		iterationNumber--;
		paletteSpriteNumber--;
		if (paletteSpriteNumber < 0)
			paletteSpriteNumber += sprites.size();
	}
	sffv1sprite_t & paletteSprite = sprites[paletteSpriteNumber];
	if (paletteSprite.dataSize > 768 && paletteSprite.data[paletteSprite.dataSize - 768 - 1] == 0x0C) {
		uint8_t * paletteData = paletteSprite.data + (paletteSprite.dataSize - 768);
		for (int i = 0; i < PALETTE_NCOLORS; i++) {
			s.colors[i] = (sffv1color_t) { *(paletteData + 3 * i), *(paletteData + 3 * i + 1), *(paletteData + 3 * i + 2) };
		}
		return s;
	}
	return palettes[currentPalette];
}

SDL_Surface * mugen::Sffv1::renderToSurface()
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
		if (colorIndex) { // if not null
				sffv1color_t & color = palette.colors[dataStart[i_byte]];
				sdlcolor = (SDL_Color) { color.red, color.green, color.blue, 0xFF };
			}
			else // apply transparency if the color value is zero
				sdlcolor = (SDL_Color) { 0, 0, 0, 0 };
		for (int runCount = runLength; runCount > 0; runCount--, i_pixel++)
			*(pixels + i_pixel) = SDL_MapRGBA(surface->format, sdlcolor.r, sdlcolor.g, sdlcolor.b, sdlcolor.a);
	}
	SDL_UnlockSurface(surface);
	
	return surface;
}

bool mugen::Sffv1::readActPalette(const char * filepath)
{
	sffv1palette_t palette;
	std::ifstream actfile;
	// reading a .act file: a Photoshop 8-bit palette
	try {
		actfile.open(filepath);
		if (actfile.fail())
			return false;
		// for some reason the colors are in reverse order
		// it didn't appear to be in the official specification though
		for (int i_palette = 0; i_palette < PALETTE_NCOLORS && actfile.good(); i_palette++) {
			palette.colors[PALETTE_NCOLORS - 1 - i_palette].red = actfile.get();
			palette.colors[PALETTE_NCOLORS - 1 - i_palette].green = actfile.get();
			palette.colors[PALETTE_NCOLORS - 1 - i_palette].blue = actfile.get();
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

void mugen::Sffv1::load()
{
	m_sprites.clear();
	for (currentPalette = 0; currentPalette < palettes.size(); currentPalette++) {
		std::unordered_map<spriteref, Sprite> currentPaletteSprites;
		for (currentSprite = 0; currentSprite < sprites.size(); currentSprite++) {
			sffv1sprite_t & sprite = sprites[currentSprite];
			spriteref ref(sprite.group, sprite.groupimage);
			currentPaletteSprites.insert(std::pair<spriteref, Sprite>(ref, Sprite(ref, renderToSurface(), currentPalette)));
		}
		m_sprites.push_back(currentPaletteSprites);
	}
}

void mugen::Sffv1::load(std::vector<spriteref>::iterator first, std::vector<spriteref>::iterator last)
{
	m_sprites.clear();
	for (currentPalette = 0; currentPalette < palettes.size(); currentPalette++) {
		std::unordered_map<spriteref, Sprite> currentPaletteSprites;
		for (; first != last; first++) {
			spriteref & ref = *first;
			currentSprite = groups[ref.group].i[ref.image];
			currentPaletteSprites.insert(std::pair<spriteref, Sprite>(ref, Sprite(ref, renderToSurface(), currentPalette)));
		}
		m_sprites.push_back(currentPaletteSprites);
	}
}
