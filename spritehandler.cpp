#include "spritehandler.h"

std::array<uint8_t, 4> extract_version(std::ifstream & fileobj)
{
	std::array<uint8_t, 4> version;
	for (int i = 0; i < 4; i++)
		version[3 - i] = fileobj.get();
	return version;
}

uint32_t read_uint32(std::ifstream & fileobj)
{
	uint32_t e = 0;
	// Little endian reading
	for (int i = 0; i < 8 * 4; i += 8)
		e |= fileobj.get() << i;
	return e;
}

uint16_t read_uint16(std::ifstream & fileobj)
{
	return fileobj.get() | (fileobj.get() << 8);
}