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