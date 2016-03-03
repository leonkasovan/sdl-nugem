#include "sffv1.h"

#include "../character.h"

#define READBUF_SIZE 12

#include <ios>
#include <fstream>
#include <iostream>
#include <array>
#include <SDL_image.h>


Sffv1::Sffv1(const char * filename)/*: character(chara)*/
{
	currentSprite = 0;
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
	while (charfile.good() && nextSubfileOffset) {
		sffv1sprite_t sprite;
		charfile.seekg(nextSubfileOffset, std::ios::beg);
		nextSubfileOffset = read_uint32(charfile);
		sprite.dataSize = read_uint32(charfile);
		sprite.axisX = read_uint16(charfile);
		sprite.axisY = read_uint16(charfile);
		sprite.group = read_uint16(charfile);
		sprite.groupimage = read_uint16(charfile);
		sprite.previousCopyIndex = read_uint16(charfile);
		uint8_t samePaletteAsPreviousByte;
		charfile >> samePaletteAsPreviousByte;
		sprite.samePaletteAsPrevious = (samePaletteAsPreviousByte != 0);
		charfile.seekg(13, std::ios::cur); // skipping the next 12 bytes, from byte 19 to byte 31
		// According to formats.txt:
		// "PCX graphic data. If palette data is available, it is the last 768 bytes."
		sprite.data = new uint8_t[sprite.dataSize];
		charfile.read((char *) sprite.data, sprite.dataSize);
		sprite.paletteData = nullptr;
		sprites.push_back(sprite);
	}

	charfile.close();
}

Sffv1::~Sffv1()
{
	for (int i = 0; i < sprites.size(); i++) {
		delete [] sprites[i].data;
		if (sprites[i].paletteData != nullptr)
			delete [] sprites[i].paletteData;
	}
}

const uint32_t Sffv1::getTotalSpriteNumber() const
{
	return nimages;
}

void Sffv1::setSprite(int n)
{
	currentSprite = n;
}

SDL_Surface * Sffv1::getSurface()
{
	sffv1sprite_t& sprite = sprites[currentSprite];
	std::ofstream outfile("testfile.pcx");
	outfile.write((char *) sprite.data, sprite.dataSize);
	outfile.close();
	SDL_RWops * imgdata = SDL_RWFromConstMem(sprite.data, sprite.dataSize);
	SDL_Surface * surface = IMG_LoadPCX_RW(imgdata);
	SDL_RWclose(imgdata);
	std::cout << SDL_GetError() << std::endl;
	return surface;
}

