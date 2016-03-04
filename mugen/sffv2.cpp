#include "sffv2.h"

#include <ios>
#include <fstream>
#include <iostream>
#include <array>

#define READBUF_SIZE 32

sffv2sprite_t readSprite(std::ifstream & fileobj)
{
	sffv2sprite_t sprite;
	sprite.groupno = read_uint16(fileobj);
	sprite.itemno = read_uint16(fileobj);
	sprite.width = read_uint16(fileobj);
	sprite.height = read_uint16(fileobj);
	sprite.axisx = read_uint16(fileobj);
	sprite.axisy = read_uint16(fileobj);
	sprite.indexlinked = read_uint16(fileobj);
	sprite.fmt = fileobj.get();
	sprite.coldepth = fileobj.get();
	sprite.dataOffset = read_uint32(fileobj);
	sprite.dataLength = read_uint32(fileobj);
	sprite.paletteIndex = read_uint16(fileobj);
	sprite.flags = read_uint16(fileobj);
	sprite.texture = nullptr;
	return sprite;
}

sffv2palette_t readPalette(std::ifstream & fileobj)
{
	sffv2palette_t palette;
	palette.groupno = read_uint16(fileobj);
	palette.itemno = read_uint16(fileobj);
	palette.numcols = read_uint16(fileobj);
	palette.indexlinked = read_uint16(fileobj);
	palette.ldataOffset = read_uint32(fileobj);
	palette.dataLength = read_uint32(fileobj);
	return palette;
}

Sffv2::Sffv2(const char * filename)
{
	currentSprite = 0;
	ldata = nullptr;
	tdata = nullptr;
	readFile(filename);
}

Sffv2::~Sffv2()
{
	if (ldata != nullptr)
		delete [] ldata;
	if (tdata != nullptr)
		delete [] tdata;
}

void Sffv2::readFile(const char * filename)
{
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
	// Reserved bytes
	charfile.seekg(8, std::ios::cur);
	// Compatibility version bytes
	std::array<uint8_t, 4> compatver = extract_version(charfile);
	charfile.seekg(8, std::ios::cur);
	uint32_t first_sprite_offset = read_uint32(charfile);
	nsprites = read_uint32(charfile);
	uint32_t first_palette_offset = read_uint32(charfile);
	npalettes = read_uint32(charfile);

	// ldata (literal data block) information
	uint32_t ldata_offset = read_uint32(charfile);
	ldataLength = read_uint32(charfile);
	if (ldataLength > 0)
		ldata = new uint8_t[ldataLength];
	// Copying ldata into memory
	fileptr = charfile.tellg();
	charfile.seekg(ldata_offset, std::ios::beg);
	charfile.read((char *) ldata, ldataLength);
	charfile.seekg(fileptr, std::ios::beg);

	// tdata (translated data block) information: must be translated during load...
	uint32_t tdata_offset = read_uint32(charfile);
	tdataLength = read_uint32(charfile);
	if (tdataLength > 0)
		tdata = new uint8_t[tdataLength];
	// Copying tdata into memory
	fileptr = charfile.tellg();
	charfile.seekg(tdata_offset, std::ios::beg);
	charfile.read((char *) tdata, tdataLength);
	charfile.seekg(fileptr, std::ios::beg);

	// Going to first sprite data
	charfile.seekg(first_sprite_offset, std::ios::beg);
	for (int i_sprite = 0; i_sprite < nsprites; i_sprite++) {
		sprites.push_back(readSprite(charfile));
	}

	// Going to first palette data
	charfile.seekg(first_palette_offset, std::ios::beg);
	for (int i_palette = 0; i_palette < npalettes; i_palette++) {
		palettes.push_back(readPalette(charfile));
	}

	charfile.close();
}

const uint32_t Sffv2::getTotalSpriteNumber() const
{
	return nsprites;
}

void Sffv2::setSprite(int n)
{
	currentSprite = n;
}

void Sffv2::outputColoredPixel(uint8_t color, const uint32_t indexPixel, const sffv2palette_t & palette, SDL_Surface * surface, const uint32_t surfaceSize)
{
	if (indexPixel < surfaceSize) {
		uint32_t * pixels = (uint32_t *) surface->pixels;
		uint8_t * colorarray = ldata + (palette.ldataOffset + color * 4);
		uint8_t red, green, blue, alpha;
		red = colorarray[0];
		green = colorarray[1];
		blue = colorarray[2];
		if (color)
			alpha = 0xFF;
		else
			alpha = 0x00;
		* (pixels + indexPixel) = SDL_MapRGBA(surface->format, red, green, blue, alpha);
	}
}

SDL_Surface * Sffv2::getSurface()
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
	sffv2sprite_t & sprite = sprites[currentSprite];
	SDL_Surface * surface = SDL_CreateRGBSurface(0, sprite.width, sprite.height, 32, rmask, gmask, bmask, amask);  // using the defaults masks
	// 		SDL_LockSurface(surface);
	uint8_t * sdata = ldata + sprite.dataOffset;
	sffv2palette_t & palette = palettes[sprite.paletteIndex];
	uint64_t indexPixel = 0;
	uint32_t surfaceSize = (uint32_t)(surface->w * surface->h);
	uint32_t i_byte = 0;
	switch (sprite.fmt) {
	case 0: // raw
		for (i_byte = 4; i_byte < sprite.dataLength; i_byte++, indexPixel++) {
			outputColoredPixel(sdata[i_byte], indexPixel, palette, surface, surfaceSize);
		}
		break;
	case 1: // invalid
		break;
	case 2: // RLE8 (Run-Length Encoding at 8 bits-per-pixel pixmap)
		for (i_byte = 4; i_byte < sprite.dataLength; i_byte++) {
			uint8_t first_byte = sdata[i_byte];
			if ((first_byte & 0xC0) == 0x40) {  // in the case of a RLE control packet
				i_byte++;
				uint8_t color = sdata[i_byte]; // <- the next byte
				uint8_t run_length = (first_byte & 0x3F);
				for (int run_count = 0; run_count < run_length; run_count++, indexPixel++)
					outputColoredPixel(color, indexPixel, palette, surface, surfaceSize);
			}
			else
				outputColoredPixel(first_byte, indexPixel++, palette, surface, surfaceSize);
		}
		break;
	case 3: // RLE5 (5 bits-per-pixel pixmaps)
		for (i_byte = 4; i_byte < sprite.dataLength; i_byte++) {
			uint8_t run_length = sdata[i_byte];
			i_byte++;
			uint8_t data_length = sdata[i_byte];
			uint8_t color;
			if (data_length & 0x80) // testing the color bit
				color = sdata[++i_byte];
			else
				color = 0;
			for (int run_count = 0; run_count < run_length; run_count++, indexPixel++)
				outputColoredPixel(color, indexPixel, palette, surface, surfaceSize);
			for (int bytes_processed = 0; bytes_processed < (data_length & 0x7F) - 1; bytes_processed++) {
				uint8_t byte = sdata[++i_byte];
				color = byte & 0x1F;
				run_length = byte >> 5;
				for (uint8_t run_count = 0; run_count < run_length; run_count++, indexPixel++)
					outputColoredPixel(color, indexPixel, palette, surface, surfaceSize);
			}
		}
		break;
	case 4: // LZ5
		// see the documentation on LZ5 compression: https://web.archive.org/web/20141230125932/http://elecbyte.com/wiki/index.php/LZ5
		// TODO Fix whatever is wrong here
		uint32_t shortlzpackets = 1;
		uint8_t recycledbits = 0;
		// first 4 bytes are the size of uncompressed data
		// so we skip them
		for (i_byte = 4; i_byte < sprite.dataLength; i_byte++) {
			uint8_t control_packet = sdata[i_byte];
			uint8_t i_data_packet;
			for (i_data_packet = 0; i_data_packet < 8 && i_byte < sprite.dataLength; i_data_packet++) {
				uint8_t flag = control_packet & (1 << i_data_packet); // value 0-> it is a RLE packet, value 1-> LZ packet
				i_byte++;
				if (!flag) { // RLE packet (short or long)
					uint8_t color = sdata[i_byte] & 0x1F;
					uint32_t run_length = sdata[i_byte] & 0xE0;
					int run_count;
					if (run_length > 0) { // short RLE packet
						run_length >>= 5; // the color runs in 1 to 7 pixels, so it's mapped on 3 bits all right
					}
					else {   // long RLE packet
						i_byte++;
						run_length = sdata[i_byte] + 8;
						// the color runs in 8 to 263 times
					}
					for (run_count = 0; run_count < run_length; run_count++, indexPixel++)
						outputColoredPixel(color, indexPixel, palette, surface, surfaceSize);
				}
				else {   // LZ packet (short or long)
					uint32_t copylength = sdata[i_byte] & 0x3F; // bits 0-5
					uint32_t * pixels = (uint32_t *) surface->pixels;
					uint16_t offset = 0;
					if (copylength) { // short LZ packet, if initial copy length is not null
						copylength += 1; // <- see the docs
						// How recycled bits work:
// 						bits 6-7: recycled bits of short LZ packet 4k + 1
// 						bits 4-5: recycled bits of short LZ packet 4k + 2
// 						bits 2-3: recycled bits of short LZ packet 4k + 3
// 						bits 0-1: recycled bits of short LZ packet 4k + 4
						if (shortlzpackets % 4 == 0) { // use the recycled bits
							recycledbits |= (sdata[i_byte] & 0xC0) >> 6;
							offset = recycledbits + 1;
							// new recycled bits cycle
							recycledbits = 0;
						}
						else { // read the extra byte
							recycledbits |= (sdata[i_byte] & 0xC0) >> (2 * ((shortlzpackets - 1) % 4));
							i_byte++;
							offset = sdata[i_byte] + 1;
						}
						shortlzpackets++;
					}
					else {   // long LZ packet
						offset = sdata[i_byte] << 2; // since the 0x3F-masked bits are null, there is only the 0xC0-masked bits left
						// ^ these are the highest 2 bits out of the 10-bit offset
						i_byte++;
						offset |= sdata[i_byte]; // low 8 bits of the 10-bit offset
						offset += 1;
						i_byte++;
						copylength = sdata[i_byte] + 3; // value range: 8 to 263
					}
					// memory copy
					// stay safe...
					// If the length is greater than the offset, then the copy pointer must go back to the beginning of the source when it reaches the full length
					// According to the Nomen developper
					// So that is why the offset has a factor here
					for (int i_pixel = 0; i_pixel < copylength && indexPixel < surfaceSize; i_pixel++, indexPixel++) {
						uint32_t offsetFromBeginning;
						if (offset)
							offsetFromBeginning = offset * (1 + i_pixel / offset);
						else // the offset should never be null though...
							offsetFromBeginning = i_pixel; // just copy the same pixel over and over again I guess
						* (pixels + indexPixel) = * (pixels + indexPixel - offsetFromBeginning);
					}
				}
			}
		}
		break;
	}
	return surface;
}


