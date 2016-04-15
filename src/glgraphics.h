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

#ifndef GLGRAPHICS_H
#define GLGRAPHICS_H

#include <GL/gl.h>
#include <SDL.h>

struct GlTexture {
	GLuint tid = 0;
	int w;
	int h;
	GlTexture(): tid(0), w(0), h(0) {};
	GlTexture(GLuint tid, int w, int h): tid(tid), w(w), h(h) {};
	~GlTexture();
};

class Game;

class GlGraphics
{
public:
	GlGraphics();
	~GlGraphics();
	void initialize(Game * game, SDL_Window* window);
	void finish();
	void clear();
	GlTexture surfaceToTexture(const SDL_Surface * surface);
	void render2DTexture(GlTexture& texture, const SDL_Rect * dstrect);
	void display();
	void windowSize(int* width, int* height);
private:
	SDL_Window * m_window;
	SDL_GLContext m_sdlglctx;
	Game * m_game;
};

#endif // GLGRAPHICS_H
