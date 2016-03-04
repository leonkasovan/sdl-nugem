#include "sffv1.h"

#include "../character.h"

#define ACT_PALETTES_NUMBER 256
#define READBUF_SIZE 12

#include <ios>
#include <fstream>
#include <iostream>
#include <array>
#include <SDL_image.h>


Sffv1::Sffv1(Character & chara, const char* filename): character(chara)
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
		sprites.push_back(sprite);
	}
	charfile.close();
	// Now we have to read the palette files
	bool continueLoop = true;
	for (int i = 1; continueLoop && i <= 12; i++) {
		std::string keyname = "pal";
		keyname += std::to_string(i);
		try {
			std::string filename = (std::string) character.getdef().at("Files").at("keyname");
		}
		catch (std::out_of_range) {
			break;
		}
		continueLoop = readActPalette(filename);
	}
}

Sffv1::~Sffv1()
{
	for (int i = 0; i < sprites.size(); i++)
		delete [] sprites[i].data;
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
	return surface;
}

bool Sffv1::readActPalette(const char * filepath)
{
	ActPalette palette;
	try {
		std::ifstream actfile(filepath);
		if (actfile.fail())
			return false;
		for (int i_palette = 0; i_palette < ACT_PALETTES_NUMBER && actfile.good(); i_palette++) {
			palette[i_palette] = (SDL_Color) { actfile.get(), actfile.get(), actfile.get() };
		}
		actfile.close();
	}
	catch (std::ios_base::failure()) {
		return false;
		
	}
	palettes.push_back(palette);
	return true;
}
