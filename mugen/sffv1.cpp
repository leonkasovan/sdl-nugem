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
		sprite.linkedindex = read_uint16(charfile);
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
	size_t displayedSprite = currentSprite;
	if (sprites[currentSprite].linkedindex && !sprites[currentSprite].dataSize)
		displayedSprite = sprites[currentSprite].linkedindex;
	sffv1sprite_t & sprite = sprites[displayedSprite];
	std::ofstream outfile("testfile.pcx");
	outfile.write((char *) sprite.data, sprite.dataSize);
	outfile.close();
	SDL_Surface * surface = nullptr;
	// check if the image already has palette data
	bool hasPalette = false;
	if (sprite.dataSize > 768)
		hasPalette = (sprite.data[sprite.dataSize - 1 - 768] == 0x0C);
	if (!hasPalette) { // if the color has no palette data, we apply one
		unsigned int width = *((uint16_t *) sprite.data + 4) - *((uint16_t *) sprite.data + 2); // Xend - Xstart
		unsigned int height = *((uint16_t *) sprite.data + 5) - *((uint16_t *) sprite.data + 3); // Yend - Ystart
		surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
		sffv1palette_t & palette = palettes[0];
		uint32_t * currentPixel = (uint32_t *) surface->pixels;
		uint8_t * readPixel = (uint8_t *) sprite.data + 128;
		// TODO fix this mess
		for (size_t i = 0; i < width * height; i++) {
			uint8_t colindex;
			if (*readPixel & 0xC0) { // RLE decompression
				uint8_t run_length = *readPixel & 0x3F;
				readPixel++;
				colindex = *readPixel;
				uint32_t color;
				if (colindex)
					color = SDL_MapRGB(surface->format, palette.colors[colindex].red, palette.colors[colindex].green, palette.colors[colindex].blue);
				else
					color = SDL_MapRGBA(surface->format, 0, 0, 0, 0);
				for (uint8_t run_count = 0; run_count < run_length - 1; run_count++) {
					*currentPixel = color;
					currentPixel++;
					i++;
				}
			}
			else { // straight pixel byte
				colindex = *readPixel;
				// Applying transparency: color 0
				if (colindex)
					*currentPixel = SDL_MapRGB(surface->format, palette.colors[colindex].red, palette.colors[colindex].green, palette.colors[colindex].blue);
				else
					*currentPixel = SDL_MapRGBA(surface->format, 0, 0, 0, 0);
				currentPixel++, readPixel++;
			}
		}
	}
	else { // if the color already has its own palette data
		// use SDL to read it
		SDL_RWops * imgdata = SDL_RWFromConstMem(sprite.data, sprite.dataSize);
		SDL_Surface * pcx_surface = IMG_LoadPCX_RW(imgdata);
		SDL_RWclose(imgdata);
		// Converting to a 32-bit surface with transparency
		surface = SDL_CreateRGBSurface(0, pcx_surface->w, pcx_surface->h, 32, rmask, gmask, bmask, amask);
		sffv1palette_t & palette = palettes[0];
		uint32_t * currentPixel = (uint32_t *) surface->pixels;
		uint8_t * copiedPixel = (uint8_t *) pcx_surface->pixels;
		for (size_t i = 0; i < surface->w * surface->h; i++, currentPixel++, copiedPixel++) {
			uint8_t bytevalue = *copiedPixel;
			SDL_Color * copiedColor = pcx_surface->format->palette->colors + bytevalue;
			// Applying transparency: color 0
			if (bytevalue)
				*currentPixel = SDL_MapRGB(surface->format, copiedColor->r, copiedColor->g, copiedColor->b);
			else
				*currentPixel = SDL_MapRGBA(surface->format, 0, 0, 0, 0);
		}
		SDL_FreeSurface(pcx_surface);
	}
	return surface;
}

bool Sffv1::readActPalette(const char * filepath)
{
	sffv1palette_t palette;
	try {
		std::ifstream actfile(filepath);
		if (actfile.fail())
			return false;
		for (int i_palette = 0; i_palette < ACT_PALETTES_NUMBER && actfile.good(); i_palette++) {
			palette.colors[i_palette] = { actfile.get(), actfile.get(), actfile.get() };
		}
		actfile.close();
	}
	catch
		(std::ios_base::failure()) {
		return false;
	}
	palettes.push_back(palette);
	return true;
}
