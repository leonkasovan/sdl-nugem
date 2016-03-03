#include "game.h"
#include "character.h"

#include <iostream>

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
	/* Deinitialize everything */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Game::run()
{
	std::vector<Character> characters;
	Character * currentChar = nullptr;
	isprite = 1;
	SDL_Event e;
	currentChar = new Character("Sakura");
	SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00); // clear color: black
	// Main game loop
	while (!SDL_QuitRequested()) {
		while (SDL_PollEvent(&e) != 0) {
			currentChar->handleEvent(e);
		}
		SDL_RenderClear(renderer);
		//Place your simulation code and rendering code here
		currentChar->render(renderer);
		SDL_RenderPresent(renderer);
		SDL_Delay(1000 / 60);  // 60 fps
	}
	if (currentChar)
		delete currentChar;
}
