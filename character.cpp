#include "character.h"

#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <ios>
#include <SDL.h>
#include "mugen/sffv1.h"
#include "mugen/sffv2.h"

Character::Character(const char * charid): id(charid)
{
	texture = nullptr;
	currentSprite = 1;
	spriteHandler = nullptr;
	needSpriteRefresh = true;
	definitionfilename = "chars/" + id + "/" + id + ".def";
	std::ifstream defs(definitionfilename);
	std::string line;
	while (std::getline(defs, line)) {
		// Processing the line
		
		// Ignore comments
	}
	spritefilename = "chars/" + id + "/" + id + ".sff";
	defs.close();
	spriteHandler = new Sffv2(spritefilename.c_str());
}

Character::~Character()
{
	if (spriteHandler)
		delete spriteHandler;
	if (texture)
		SDL_DestroyTexture(texture);
}

void Character::render(SDL_Renderer * renderer)
{
	// TODO only get surface when needed (check currentSprite)
	if (needSpriteRefresh) {
		if (texture)
			SDL_DestroyTexture(texture);
		spriteHandler->setSprite(currentSprite);
		SDL_Surface * surface = spriteHandler->getSurface();
		width = surface->w;
		height = surface->h;
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		needSpriteRefresh = false;
	}
	SDL_Rect DestR;
	DestR.x = 0;
	DestR.y = 0;
	DestR.w = width;
	DestR.h = height;
	SDL_RenderCopy(renderer, texture, nullptr, &DestR);
}

void Character::handleEvent(const SDL_Event e)
{
	const int32_t nsprites = spriteHandler->getTotalSpriteNumber();
	if (e.type == SDL_KEYDOWN) {
		//Select surfaces based on key press
		switch (e.key.keysym.sym) {
		case SDLK_UP:
			currentSprite++;
			needSpriteRefresh = true;
			break;

		case SDLK_DOWN:
			currentSprite--;
			needSpriteRefresh = true;
			break;
		}

		currentSprite = (currentSprite + nsprites) % nsprites;
	}
}


