#include "game.h"
#include "character.h"

#include <iostream>
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
	                          0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

Game::~Game()
{
	for (auto character = characters.begin(); character != characters.end(); character++)
		delete *character;
	// SDL deinitialization
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Game::run()
{
	findCharacters();
	unsigned int currentCharacter = 0;
	isprite = 1;
	SDL_Event e;
	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00); // clear color: black
	// Main game loop
	while (!SDL_QuitRequested()) {
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
			characters[currentCharacter]->handleEvent(e);
		}
		SDL_RenderClear(renderer);
		//Place your simulation code and rendering code here
		characters[currentCharacter]->render(renderer);
		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);  // 60 fps
	}
}

void Game::findCharacters()
{
	characters.push_back(new Character("kfm"));
	characters.push_back(new Character("kfm720"));
	characters.push_back(new Character("Sakura"));
	characters.push_back(new Character("ryusfa"));
}

