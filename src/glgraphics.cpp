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

#include "glgraphics.hpp"

#include <iostream>

namespace Nugem {

GlGraphics::GlGraphics(Window &window): mWindow(window)
{
	//Use OpenGL 3.1 core
	
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	
	// Get window context
	
	mSDLGlCtx = mWindow.createGlContext();
}

GlGraphics::~GlGraphics()
{
}

void GlGraphics::initialize(Game * game)
{
   mGame = game;
	
    glViewport(0, 0, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glPushMatrix(); //Start phase
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glOrtho(0, 1, 1, 0, -1, 1);
}

void GlGraphics::finish()
{
	SDL_GL_DeleteContext(mSDLGlCtx);
}

void GlGraphics::clear()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	   mCurrentZ = 0;
}

GlTexture GlGraphics::surfaceToTexture(const SDL_Surface * surface)
{
	GLuint tid = 0;
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	
	int Mode = GL_RGB;
	
	if(surface->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}
	
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	
	GlTexture tx(tid, surface->w, surface->h);
	
	return tx;
}

void GlGraphics::render2DTexture(GlTexture & texture, const SDL_Rect * dstrect)
{
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, texture.tid);
	
	int X, Y, Width, Height;
	if (dstrect != nullptr) {
		X = dstrect->x;
		Y = dstrect->y;
		if (dstrect->w >= 0)
			Width = dstrect->w;
		else
			Width = texture.w;
		if (dstrect->h >= 0)
			Height = dstrect->h;
		else
			Height = texture.h;
	}
	else {
		X = 0;
		Y = 0; 
		Width = texture.w;
		Height = texture.h;
	}
	
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(X, Y, 0);
		glTexCoord2f(1, 0); glVertex3f(X + Width, Y, 0);
		glTexCoord2f(1, 1); glVertex3f(X + Width, Y + Height, 0);
		glTexCoord2f(0, 1); glVertex3f(X, Y + Height, 0);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

void GlGraphics::display()
{
	mWindow.swapGlWindow();
}

std::unordered_map<GLuint, unsigned int> GlTexture::useCounters;

GlTexture::GlTexture(GLuint tid, int w, int h): tid(tid), w(w), h(h)
{
	if (tid)
		useCounters[tid]++;
}


GlTexture::~GlTexture()
{
	if (tid) {
		useCounters[tid]--;
		if (useCounters[tid] <= 0) {
			glDeleteTextures(1, &tid);
			useCounters.erase(tid);
		}
	}
}

GlTexture::GlTexture(const GlTexture & glTexture)
{
	tid = glTexture.tid;
	w = glTexture.w;
	h = glTexture.h;
	if (tid)
		useCounters[tid]++;
}

GlTexture::GlTexture(GlTexture && glTexture): tid(0), w(0), h(0)
{
	std::swap(tid, glTexture.tid);
	std::swap(w, glTexture.w);
	std::swap(h, glTexture.h);
}

}
