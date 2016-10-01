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
	mWidth = bounds.w;
	mHeight = bounds.h;
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

size_t Window::width()
{
	return mWidth;
}

size_t Window::height()
{
	return mHeight;
}

void Window::processSDLEvent(const SDL_Event &e)
{
	if (e.type == SDL_WINDOWEVENT) {
		switch (e.window.event) {
        case SDL_WINDOWEVENT_SHOWN:
//             SDL_Log("Window %d shown", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
//             SDL_Log("Window %d hidden", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
//             SDL_Log("Window %d exposed", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MOVED:
//             SDL_Log("Window %d moved to %d,%d",
//                     event->window.windowID, event->window.data1,
//                     event->window.data2);
			resizeToFullscreen();
            break;
        case SDL_WINDOWEVENT_RESIZED:
//             SDL_Log("Window %d resized to %dx%d",
//                     event->window.windowID, event->window.data1,
//                     event->window.data2);
			mWidth = e.window.data1;
			mHeight = e.window.data2;
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
//             SDL_Log("Window %d size changed to %dx%d",
//                     event->window.windowID, event->window.data1,
//                     event->window.data2);
			mWidth = e.window.data1;
			mHeight = e.window.data2;
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
//             SDL_Log("Window %d minimized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
//             SDL_Log("Window %d maximized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_RESTORED:
//             SDL_Log("Window %d restored", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_ENTER:
//             SDL_Log("Mouse entered window %d",
//                     event->window.windowID);
            break;
        case SDL_WINDOWEVENT_LEAVE:
//             SDL_Log("Mouse left window %d", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
//             SDL_Log("Window %d gained keyboard focus",
//                     event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
//             SDL_Log("Window %d lost keyboard focus",
//                     event->window.windowID);
            break;
        case SDL_WINDOWEVENT_CLOSE:
//             SDL_Log("Window %d closed", event->window.windowID);
            break;
        default:
//             SDL_Log("Window %d got unknown event %d",
//                     event->window.windowID, event->window.event);
            break;
        }
	}
}

}
