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

#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "input.h"
#include "scene.h"
#include "character.h"
#include "glgraphics.h"

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

class Scene;
class Player;

class Game
{
public:
	Game();
	~Game();
	void run();
	GlGraphics & glGraphics() { return m_glGraphics; };
	Scene * currentScene();
	InputManager & inputManager();
	void setScene(Scene * newScene);
protected:
	void update();
	InputManager m_inputManager;
private:
	SDL_Window * m_window;
	SDL_GLContext m_glContext;
	Scene * m_currentScene;
	GlGraphics m_glGraphics;
};

#endif // GAME_H

