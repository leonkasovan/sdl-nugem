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
#include "character.h"

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
protected:
	unsigned int w_width;
	unsigned int w_height;
	void findCharacters();
	void update(int32_t dt);
	InputManager inputManager;
private:
	SDL_Window * window;
	SDL_Renderer * renderer;
	uint32_t isprite;
	std::vector<Character> characters;
	size_t currentCharacter;
};

#endif // GAME_H

