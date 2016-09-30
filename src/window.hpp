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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL.h>
#include <string>
#include <tuple>

namespace Nugem {

class Window {
public:
	Window();
	~Window();
	void processSDLEvent(const SDL_Event &);
	void resizeToFullscreen();
	void swapGlWindow();
	SDL_GLContext createGlContext();
	size_t width();
	size_t height();
	
	operator bool() const;
private:
	const static unsigned int DEFAULT_WINDOW_WIDTH = 640;
	const static unsigned int DEFAULT_WINDOW_HEIGHT = 480;
	SDL_Window *mSDLWindow;
	std::string mTitle = "NUGEM";
	size_t mWidth;
	size_t mHeight;
};

}

#endif // WINDOW_HPP
