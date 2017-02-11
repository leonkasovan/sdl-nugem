/*
 * Copyright (c) 2016 Victor Nivet
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

using namespace std;

namespace Nugem {

SurfaceDrawer::SurfaceDrawer(size_t width, size_t height)
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
	m_surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
	SDL_LockSurface(m_surface);
}

SurfaceDrawer::~SurfaceDrawer()
{
	SDL_UnlockSurface(m_surface);
}

uint32_t SurfaceDrawer::rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) const
{
	return SDL_MapRGBA(m_surface->format, red, green, blue, alpha);
}

SDL_Surface * SurfaceDrawer::operator()()
{
	draw(static_cast<uint32_t *>(m_surface->pixels), m_surface->w, m_surface->h);
	return m_surface;
}

namespace Mugen {

array<uint8_t, 4> extract_version(ifstream & fileobj)
{
	array<uint8_t, 4> version;
	for (int i = 0; i < 4; i++)
		fileobj >> version[3 - i];
	return version;
}

uint32_t read_uint32(ifstream & fileobj)
{
	uint32_t e = 0;
	// Little endian reading
 	for (int i = 0; i < 8 * 4; i += 8)
 		e |= fileobj.get() << i;
	return e;
}

uint16_t read_uint16(ifstream & fileobj)
{
	return fileobj.get() | (fileobj.get() << 8);
}

Sprite::Sprite(Spriteref reference, SDL_Surface * surface, int palette): m_ref(reference), m_npalette(palette), m_surface(surface)
{
}

Sprite::~Sprite()
{
	if (m_surface != nullptr) {
		SDL_FreeSurface(m_surface);
		m_surface = nullptr;
	}
}

Sprite::Sprite(const Sprite & originalSprite): m_surface(nullptr)
{
	m_ref = originalSprite.ref();
	m_surface = copySurface(originalSprite.m_surface);
}

Sprite::Sprite(Sprite && originalSprite): m_surface(nullptr)
{
	swap(m_ref, originalSprite.m_ref);
	swap(m_surface, originalSprite.m_surface);
}

Sprite & Sprite::operator=(const Sprite & originalSprite)
{
	m_ref = originalSprite.ref();
	m_surface = copySurface(originalSprite.m_surface);
	return *this;
}

Sprite & Sprite::operator=(Sprite && originalSprite)
{
	swap(m_ref, originalSprite.m_ref);
	swap(m_surface, originalSprite.m_surface);
	return *this;
}

SDL_Surface * Sprite::copySurface(SDL_Surface * surface)
{
	return SDL_ConvertSurface(surface, surface->format, 0);
}

SpriteLoader::SpriteLoader()
{
}

void SpriteLoader::initialize(const std::string & sffpath, const std::string & palettesFile)
{
	m_sffFile = sffpath;
	m_palettesFile = palettesFile;
	
	// Determining sprite version
	{
		char readbuf[12];
		ifstream spritefile(sffpath);
		spritefile.read(readbuf, 12);
		if (strcmp(readbuf, "ElecbyteSpr")) {
			return;
		}
		m_sffVersion = extract_version(spritefile);
		spritefile.close();
	}
}

vector< unordered_map< Spriteref, Sprite > > SpriteLoader::load()
{
	SpriteHandler * handler = createHandler();
	handler->load();
	vector< unordered_map< Spriteref, Sprite > > s = handler->sprites();
	delete handler;
	return s;
}

vector< unordered_map< Spriteref, Sprite > > SpriteLoader::load(vector< Spriteref >::iterator first, vector< Spriteref >::iterator last)
{
	SpriteHandler * handler = createHandler();
	handler->load(first, last);
	vector< unordered_map< Spriteref, Sprite > > s = handler->sprites();
	delete handler;
	return s;
}

unordered_map< Spriteref, Sprite > SpriteLoader::loadForPalette(int palette)
{
	SpriteHandler * handler = createHandler();
	handler->load();
	vector< unordered_map< Spriteref, Sprite > > s = handler->sprites();
	delete handler;
	return s[palette];
}

SpriteHandler * SpriteLoader::createHandler()
{
	if (m_sffVersion[0] >= 2)
		return new Sffv2(m_sffFile.c_str());
	else
		return new Sffv1(m_sffFile.c_str(), m_palettesFile.c_str());
}

bool SpriteLoader::isInitialized() const
{
	return (m_sffFile.length() > 0);
}

}
}

