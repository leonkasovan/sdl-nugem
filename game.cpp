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

#include "game.h"
#include "character.h"

#include <iostream>
#include <dirent.h>
#include <SDL_image.h>

// To be replaced ? it's not really cross-platform
#include <dirent.h>

Game::Game()
{
	// Initialize window constants
	w_width = DEFAULT_WINDOW_WIDTH;
	w_height = DEFAULT_WINDOW_HEIGHT;
	/* Initialize SDL */
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("NEGUM",
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          w_width, w_height,
	                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	// SDL_image initialization
	IMG_Init(0);
}

Game::~Game()
{
// 	for (auto character = characters.begin(); character != characters.end(); character++)
// 		delete *character;
	// SDL_image deinitialization
	IMG_Quit();
	// SDL deinitialization
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Game::update(int32_t dt)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_KEYDOWN) {
			//Select surfaces based on key press
			switch (e.key.keysym.sym) {
			case SDLK_LEFT:
				currentCharacter--;
				break;

			case SDLK_RIGHT:
				currentCharacter++;
				break;
			}

			currentCharacter = (currentCharacter + characters.size()) % characters.size();
		}
		inputManager.processSDLEvent(e);
		characters[currentCharacter].handleEvent(e);
	}
	SDL_RenderClear(renderer);
	//Place your simulation code and rendering code here
	characters[currentCharacter].render(renderer);
	SDL_RenderPresent(renderer);
}

void Game::run()
{
	findCharacters();
	currentCharacter = 0;
	isprite = 1;
	SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xC6, 0x00); // clear color: black
	uint32_t tick;
	uint32_t prevtick = SDL_GetTicks();
	uint32_t tickdelay = 1000 / 60; // 60 fps
	// Main game loop
	while (!SDL_QuitRequested()) {
		tick = SDL_GetTicks();
		uint32_t dt = tick - prevtick;
		if (dt >= tickdelay) {
			update(dt);
			prevtick = tick;
		}
	}
}

void Game::findCharacters()
{
	DIR * chardir = nullptr;
	struct dirent * chardirent = nullptr;
	chardir = opendir("chars");
	if (chardir != nullptr) {
		while (chardirent = readdir(chardir)) {
			const char * name = chardirent->d_name;
			if (name[0] == '.')
				continue;
			try {
			characters.push_back(Character(name));
			}
			catch (CharacterLoadException & error) {
				std::cerr << "Couldn't load character " << name << ": " << error.what() << std::endl;
			}
		}
		closedir(chardir);
	}
}

