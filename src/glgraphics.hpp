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

#ifndef GLGRAPHICS_H
#define GLGRAPHICS_H

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <unordered_map>
#include "window.hpp"

namespace Nugem {

struct GlTexture {
	GLuint tid = 0;
	int w;
	int h;
	GlTexture(GLuint tid, int w, int h);
	GlTexture(const GlTexture & glTexture);
	GlTexture(GlTexture && glTexture);
	~GlTexture();
private:
	static std::unordered_map<GLuint, unsigned int> useCounters;
};

class Game;

class GlGraphics
{
public:
	GlGraphics(Window &);
	~GlGraphics();
	void initialize(Game * game);
	void finish();
	void clear();
	GlTexture surfaceToTexture(const SDL_Surface * surface);
	void render2DTexture(GlTexture& texture, const SDL_Rect * dstrect);
	void display();
private:
	Window &mWindow;
	Game * mGame;
	SDL_GLContext mSDLGlCtx;
	GLuint gProgramID = 0;
	GLint gVertexPos2DLocation = -1;
	GLuint gVBO = 0;
	GLuint gIBO = 0;
	GLfloat mCurrentZ;
};

}

#endif // GLGRAPHICS_H
