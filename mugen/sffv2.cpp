#include "sffv2.h"

#include <ios>
#include <fstream>
#include <iostream>
#include <array>

#define READBUF_SIZE 32

inline std::array<uint8_t, 4> extract_version(std::ifstream & fileobj)
{
	std::array<uint8_t, 4> version;
	for (int i = 0; i < 4; i++)
		version[3 - i] = fileobj.get();
	return version;
}


// Little endian to big endian
inline uint32_t read_uint32(std::ifstream & fileobj)
{
	uint32_t e = 0;
	// Little endian reading
	for (int i = 0; i < 4; i++)
		e |= fileobj.get() << (8 * i);
	return e;
}

inline uint16_t read_uint16(std::ifstream & fileobj)
{
	return fileobj.get() | (fileobj.get() << 8);
}

inline sprite_t read_sprite(std::ifstream & fileobj)
{
	sprite_t sprite;
	sprite.groupno = read_uint16(fileobj);
	sprite.itemno = read_uint16(fileobj);
	sprite.width = read_uint16(fileobj);
	sprite.height = read_uint16(fileobj);
	sprite.axisx = read_uint16(fileobj);
	sprite.axisy = read_uint16(fileobj);
	sprite.indexlinked = read_uint16(fileobj);
	sprite.fmt = fileobj.get();
	sprite.coldepth = fileobj.get();
	sprite.data_offset = read_uint32(fileobj);
	sprite.data_length = read_uint32(fileobj);
	sprite.palette_index = read_uint16(fileobj);
	sprite.flags = read_uint16(fileobj);
	sprite.texture = nullptr;
	return sprite;
}

inline pallet_t read_pallet(std::ifstream & fileobj)
{
	pallet_t pallet;
	pallet.groupno = read_uint16(fileobj);
	pallet.itemno = read_uint16(fileobj);
	pallet.numcols = read_uint16(fileobj);
	pallet.indexlinked = read_uint16(fileobj);
	pallet.ldata_offset = read_uint32(fileobj);
	pallet.data_length = read_uint32(fileobj);
	return pallet;
}

Sffv2::Sffv2(const char * filename)
{
	currentsprite = 0;
	ldata = nullptr;
	tdata = nullptr;
	uint32_t fileptr;
	uint8_t * readbuf = new uint8_t[READBUF_SIZE];
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
	uint32_t first_pallet_offset = read_uint32(charfile);
	npallets = read_uint32(charfile);

	// ldata (literal data block) information
	uint32_t ldata_offset = read_uint32(charfile);
	ldata_length = read_uint32(charfile);
	if (ldata_length > 0)
		ldata = new uint8_t[ldata_length];
	// Copying ldata into memory
	fileptr = charfile.tellg();
	charfile.seekg(ldata_offset, std::ios::beg);
	charfile.read((char *) ldata, ldata_length);
	charfile.seekg(fileptr, std::ios::beg);

	// tdata (translated data block) information: must be translated during load...
	uint32_t tdata_offset = read_uint32(charfile);
	tdata_length = read_uint32(charfile);
	if (tdata_length > 0)
		tdata = new uint8_t[tdata_length];
	// Copying tdata into memory
	fileptr = charfile.tellg();
	charfile.seekg(tdata_offset, std::ios::beg);
	charfile.read((char *) tdata, tdata_length);
	charfile.seekg(fileptr, std::ios::beg);

	// Going to first sprite data
	charfile.seekg(first_sprite_offset, std::ios::beg);
	for (int i_sprite = 0; i_sprite < nsprites; i_sprite++) {
		sprites.push_back(read_sprite(charfile));
	}

	// Going to first palette data
	charfile.seekg(first_pallet_offset, std::ios::beg);
	for (int i_pallet = 0; i_pallet < npallets; i_pallet++) {
		pallets.push_back(read_pallet(charfile));
	}

	charfile.close();
	delete [] readbuf;
}

Sffv2::~Sffv2()
{
	if (ldata != nullptr)
		delete [] ldata;
	if (tdata != nullptr)
		delete [] tdata;
}

const uint32_t Sffv2::getTotalSpriteNumber() const
{
	return nsprites;
}

void Sffv2::setSprite(int n)
{
	currentsprite = n;
}

void Sffv2::outputColoredPixelFromPallet(uint8_t color, const uint32_t index_pixel, const pallet_t & pallet, SDL_Surface * surface, const uint32_t surface_size)
{
	if (index_pixel < surface_size) {
		uint32_t * pixels = (uint32_t *) surface->pixels;
		uint8_t * colorarray = ldata + (pallet.ldata_offset + color * 4);
		uint8_t red = colorarray[0];
		uint8_t green = colorarray[1];
		uint8_t blue = colorarray[2];
		* (pixels + index_pixel) = SDL_MapRGB(surface->format, red, green, blue);
	}
}

SDL_Surface * Sffv2::getSurface()
{
	sprite_t & sprite = sprites[currentsprite];
	SDL_Surface * surface = SDL_CreateRGBSurface(0, sprite.width, sprite.height, 32, 0, 0, 0, 0);  // using the defaults masks
	// 		SDL_LockSurface(surface);
	uint8_t * sdata = ldata + sprite.data_offset;
	pallet_t & pallet = pallets[sprite.palette_index];
	uint32_t index_pixel = 0;
	uint32_t surface_size = (uint32_t)(surface->w * surface->h);
	uint32_t i_byte = 0;
	switch (sprite.fmt) {
	case 0: // raw
		for (i_byte = 0; i_byte < sprite.data_length; i_byte++, index_pixel++) {
			outputColoredPixelFromPallet(sdata[i_byte], index_pixel, pallet, surface, surface_size);
		}
		break;
	case 1: // invalid
		break;
	case 2: // RLE8 (Run-Length Encoding at 8 bits-per-pixel pixmap)
		for (i_byte = 0; i_byte < sprite.data_length; i_byte++) {
			uint8_t first_byte = sdata[i_byte];
			if ((first_byte & 0xC0) == 0x40) {  // in the case of a RLE control packet
				i_byte++;
				uint8_t color = sdata[i_byte]; // <- the next byte
				uint8_t run_length = (first_byte & 0x3F);
				for (int run_count = 0; run_count < run_length; run_count++, index_pixel++)
					outputColoredPixelFromPallet(color, index_pixel, pallet, surface, surface_size);
			}
			else
				outputColoredPixelFromPallet(first_byte, index_pixel++, pallet, surface, surface_size);
		}
		break;
	case 3: // RLE5 (5 bits-per-pixel pixmaps)
		for (i_byte = 0; i_byte < sprite.data_length; i_byte++) {
			uint8_t run_length = sdata[i_byte];
			i_byte++;
			uint8_t data_length = sdata[i_byte];
			uint8_t color;
			if (data_length & 0x80) // testing the color bit
				color = sdata[++i_byte];
			else
				color = 0;
			for (int run_count = 0; run_count < run_length; run_count++, index_pixel++)
				outputColoredPixelFromPallet(color, index_pixel, pallet, surface, surface_size);
			for (int bytes_processed = 0; bytes_processed < (data_length & 0x7F) - 1; bytes_processed++) {
				uint8_t byte = sdata[++i_byte];
				color = byte & 0x1F;
				run_length = byte >> 5;
				for (uint8_t run_count = 0; run_count < run_length; run_count++, index_pixel++)
					outputColoredPixelFromPallet(color, index_pixel, pallet, surface, surface_size);
			}
		}
		break;
	case 4: // LZ5
		// see the documentation on LZ5 compression: https://web.archive.org/web/20141230125932/http://elecbyte.com/wiki/index.php/LZ5
		// TODO Fix this
		uint32_t shortlzpackets = 1;
		uint8_t recycledbits = 0;
		// first 4 bytes are the size of uncompressed data
		// so we skip them
		for (i_byte = 4; i_byte < sprite.data_length; i_byte++) {
			uint8_t control_packet = sdata[i_byte];
			uint8_t i_data_packet;
			for (i_data_packet = 0; i_data_packet < 8 && i_byte < sprite.data_length; i_data_packet++) {
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
					for (run_count = 0; run_count < run_length; run_count++, index_pixel++)
						outputColoredPixelFromPallet(color, index_pixel, pallet, surface, surface_size);
				}
				else {   // LZ packet (short or long)
					uint16_t copylength = sdata[i_byte] & 0x1F;
					uint32_t * pixels = (uint32_t *) surface->pixels;
					uint8_t offset = 0;
					if (copylength > 0) { // short LZ packet
						copylength += 1; // <- see the docs
						// How recycled bits work:
// 								bits 6-7: recycled bits of short LZ packet 4k + 1
// 								bits 4-5: recycled bits of short LZ packet 4k + 2
// 								bits 2-3: recycled bits of short LZ packet 4k + 3
// 								bits 0-1: recycled bits of short LZ packet 4k + 4
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
						offset = sdata[i_byte] << 2; // since the 0x1F-masked bits are null, there is only 0xC0 left
						// ^ these are the highest 2 bits out of the 10-bit offset
						i_byte++;
						offset |= sdata[i_byte]; // low 8 bits of the 10-bit offset
						offset += 1;
						i_byte++;
						copylength = sdata[i_byte] + 3;
					}
					// memory copy
					// stay safe...
					// If the length is greater than the offset, then the copy pointer must go back to the beginning of the source when it reaches the full length
					// Source: Nomen, internal_sffv2_rle5-lz5_decode.h:200
					// That is why the factor for the offset is there
					for (int i_pixel = 0; i_pixel < copylength && index_pixel < surface_size; i_pixel++, index_pixel++) {
						uint32_t offset_from_beginning = offset * (1 + i_pixel / copylength);
						if (i_pixel > offset_from_beginning)
							* (pixels + index_pixel) = 0;
						else
							* (pixels + index_pixel) = * (pixels + index_pixel - offset_from_beginning);
					}
				}
			}
		}
		break;
	}
	std::cout << "pixels written: " << (int) index_pixel << "; surface size: " << (int) surface_size << "; bytes read: " << i_byte << "; data length: " << sprite.data_length << std::endl;
	return surface;
}


