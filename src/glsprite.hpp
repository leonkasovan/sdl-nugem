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

#ifndef GLSPRITE_HPP
#define GLSPRITE_HPP

#include "glgraphics.hpp"
#include <SDL.h>

namespace Nugem {

struct GlSpriteCollectionData
{
	size_t w;
	size_t h;
	size_t x;
};

class GlSpriteCollection
{
public:
	GlSpriteCollection(GLuint, std::vector<GlSpriteCollectionData> &&);
	GlSpriteCollection(GlSpriteCollection &&);
	~GlSpriteCollection();
	decltype(auto) tid() { return m_tid; };
	decltype(auto) width() { return m_totalWidth; };
	decltype(auto) height() { return m_totalHeight; };
	const std::vector<GlSpriteCollectionData> & sprites() { return m_sprites; };
private:
	GLuint m_tid;
	std::vector<GlSpriteCollectionData> m_sprites;
	GLfloat m_totalWidth;
	GLfloat m_totalHeight;
};

class GlSpriteCollectionBuilder
{
public:
	GlSpriteCollectionBuilder();
	~GlSpriteCollectionBuilder();
	size_t addSprite(const SDL_Surface *);
	GlSpriteCollection *build();
private:
	std::vector<GlSpriteCollectionData> m_spriteList;
	size_t m_maxHeight;
	size_t m_totalWidth;
	bool m_built;
	SDL_Surface *m_surface;
	GlSpriteCollection *m_result;
};

class GlSpriteDisplayer
{
public:
	GlSpriteDisplayer(GlSpriteCollection &);
	void addSprite(size_t, SDL_Rect &);
	void display(GlGraphics &);
private:
	GlSpriteCollection &m_spriteAtlas;
	GlGraphics::Positions m_positions;
	GlGraphics::TexCoords m_texCoords;
};

}

#endif // GLSPRITE_HPP

