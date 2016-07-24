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

#include "sprites.hpp"

#include "sffv1.hpp"

#include "sffv2.hpp"

#include "../character.hpp"

std::array<uint8_t, 4> mugen::extract_version(std::ifstream & fileobj)
{
	std::array<uint8_t, 4> version;
	for (int i = 0; i < 4; i++)
		version[3 - i] = fileobj.get();
	return version;
}

uint32_t mugen::read_uint32(std::ifstream & fileobj)
{
	uint32_t e = 0;
	// Little endian reading
	for (int i = 0; i < 8 * 4; i += 8)
		e |= fileobj.get() << i;
	return e;
}

uint16_t mugen::read_uint16(std::ifstream & fileobj)
{
	return fileobj.get() | (fileobj.get() << 8);
}

mugen::Sprite::Sprite(spriteref reference, SDL_Surface * surface, int palette): m_ref(reference), m_surface(surface), m_npalette(palette)
{
}

mugen::Sprite::Sprite(mugen::spriteref reference, SDL_Surface * surface): m_ref(reference), m_surface(surface), m_npalette(-1)
{
}

mugen::Sprite::~Sprite()
{
	if (m_surface != nullptr) {
		SDL_FreeSurface(m_surface);
		m_surface = nullptr;
	}
}

mugen::Sprite::Sprite(const Sprite & originalSprite): m_surface(nullptr)
{
	m_ref = originalSprite.ref();
	m_surface = copySurface(originalSprite.m_surface);
}

mugen::Sprite::Sprite(Sprite && originalSprite): m_surface(nullptr)
{
	std::swap(m_ref, originalSprite.m_ref);
	std::swap(m_surface, originalSprite.m_surface);
}

mugen::Sprite & mugen::Sprite::operator=(const Sprite & originalSprite)
{
	m_ref = originalSprite.ref();
	m_surface = copySurface(originalSprite.m_surface);
	return *this;
}

mugen::Sprite & mugen::Sprite::operator=(Sprite && originalSprite)
{
	std::swap(m_ref, originalSprite.m_ref);
	std::swap(m_surface, originalSprite.m_surface);
	return *this;
}

SDL_Surface * mugen::Sprite::copySurface(SDL_Surface * surface)
{
	return SDL_ConvertSurface(surface, surface->format, 0);
}

mugen::SpriteLoader::SpriteLoader(): m_sffFile("")
{
}

void mugen::SpriteLoader::initialize(const std::__cxx11::string & sffpath, Character * character)
{
	m_sffFile = sffpath;
	
	// Determining sprite version
	{
		char readbuf[12];
		std::ifstream spritefile(sffpath);
		spritefile.read(readbuf, 12);
		if (strcmp(readbuf, "ElecbyteSpr")) {
			return;
		}
		m_sffVersion = extract_version(spritefile);
		spritefile.close();
	}
	
	m_character = character;
}

std::vector< std::unordered_map< mugen::spriteref, mugen::Sprite > > mugen::SpriteLoader::load()
{
	SpriteHandler * handler = createHandler();
	handler->load();
	std::vector< std::unordered_map< mugen::spriteref, mugen::Sprite > > s = handler->sprites();
	delete handler;
	return s;
}

std::vector< std::unordered_map< mugen::spriteref, mugen::Sprite > > mugen::SpriteLoader::load(std::vector< spriteref >::iterator first, std::vector< spriteref >::iterator last)
{
	SpriteHandler * handler = createHandler();
	handler->load(first, last);
	std::vector< std::unordered_map< mugen::spriteref, mugen::Sprite > > s = handler->sprites();
	delete handler;
	return s;
}

std::unordered_map< mugen::spriteref, mugen::Sprite > mugen::SpriteLoader::loadForPalette(int palette)
{
	SpriteHandler * handler = createHandler();
	handler->load();
	std::vector< std::unordered_map< mugen::spriteref, mugen::Sprite > > s = handler->sprites();
	delete handler;
	return s[palette];
}

mugen::SpriteHandler * mugen::SpriteLoader::createHandler()
{
	if (m_sffVersion[0] >= 2)
		return new Sffv2(m_sffFile.c_str());
	else
		return new Sffv1(*m_character, m_sffFile.c_str());
}

bool mugen::SpriteLoader::isInitialized() const
{
	return (m_character != nullptr) && (m_sffFile.length() > 0);
}

