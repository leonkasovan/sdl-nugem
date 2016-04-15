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

#include "glgraphics.h"

GlGraphics::GlGraphics()
{
}

GlGraphics::~GlGraphics()
{
}

void GlGraphics::initialize(SDL_Window * window)
{
	m_window = window;
	m_sdlglctx = SDL_GL_CreateContext(m_window);
	glClearColor(0,0,0,1);
}

void GlGraphics::finish()
{
	SDL_GL_DeleteContext(m_sdlglctx);
}

void GlGraphics::clear()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void GlGraphics::renderSurface(SDL_Surface * surface, const SDL_Rect * srcrect, const SDL_Rect * dstrect)
{

}


