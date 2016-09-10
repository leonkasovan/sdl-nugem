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

#include "window.hpp"

namespace Nugem {

Window::Window()
{
	mSDLWindow = SDL_CreateWindow(mTitle.c_str(),
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
	                          SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
	
	resizeToFullscreen();
}

Window::~Window()
{
	SDL_DestroyWindow(mSDLWindow);
}

void Window::resizeToFullscreen()
{
	int idx = SDL_GetWindowDisplayIndex(mSDLWindow);
	SDL_Rect bounds;
	SDL_GetDisplayBounds(idx, &bounds);
	SDL_SetWindowBordered(mSDLWindow, SDL_FALSE);
	SDL_SetWindowPosition(mSDLWindow, bounds.x, bounds.y);
	SDL_SetWindowSize(mSDLWindow, bounds.w, bounds.h);
}

void Window::swapGlWindow()
{
	SDL_GL_SwapWindow(mSDLWindow);
}

SDL_GLContext Window::createGlContext()
{
	return SDL_GL_CreateContext(mSDLWindow);
}

Window::operator bool() const
{
	return mSDLWindow;
}

}
