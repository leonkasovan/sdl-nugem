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

#include "character.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <string>
#include <ios>
#include <SDL.h>
#include "mugen/sffv1.h"
#include "mugen/sffv2.h"

Character::Character(const char * charid): id(charid)
{
	texture = nullptr;
	currentSprite = 0;
	currentPalette = 0;
	spriteHandler = nullptr;
	directory = "chars/" + id;
	definitionfilename = id + ".def";
	loadCharacterDef((directory + "/" + definitionfilename).c_str());
	needSpriteRefresh = true;
}

Character::~Character()
{
	if (spriteHandler)
		delete spriteHandler;
	if (texture)
		SDL_DestroyTexture(texture);
}

const mugen::defcontents & Character::getdef() const
{
	return def;
}

const std::__cxx11::string & Character::getdir() const
{
	return directory;
}

void Character::loadCharacterDef(const char * filepath)
{
	if (spriteHandler)
		delete spriteHandler;
	def = mugen::loadDef(filepath);
	mugenversion = (std::string) def["Info"]["mugenversion"];
	spritefilename = (std::string) def["Files"]["sprite"];
	std::string spritepath = directory + "/" + spritefilename;
	std::array<uint8_t, 4> version;
	// Determining sprite version
	{
		char readbuf[12];
		std::ifstream spritefile(spritepath);
		spritefile.read(readbuf, 12);
		if (strcmp(readbuf, "ElecbyteSpr")) {
			throw std::runtime_error(std::string("Invalid sprite file: ") + spritepath);
		}
		version = extract_version(spritefile);
		spritefile.close();
	}
	if (version[0] < 2)
		spriteHandler = new Sffv1(*this, spritepath.c_str());
	else if (version[0] == 2)
		spriteHandler = new Sffv2(spritepath.c_str());
}

void Character::render(SDL_Renderer * renderer)
{
	// TODO only get surface when needed (check currentSprite)
	if (needSpriteRefresh) {
		if (texture)
			SDL_DestroyTexture(texture);
		spriteHandler->setSprite(currentSprite);
		spriteHandler->setPalette(currentPalette);
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
	const size_t nsprites = spriteHandler->getTotalSpriteNumber();
	const size_t npalettes = spriteHandler->getTotalPaletteNumber();
	if (e.type == SDL_KEYDOWN) {
		// Select surfaces based on key press
		
		// For now...
		switch (e.key.keysym.sym) {
		// Changing sprites
		case SDLK_UP:
			currentSprite++;
			needSpriteRefresh = true;
			break;

		case SDLK_DOWN:
			currentSprite--;
			needSpriteRefresh = true;
			break;
			
		
		case SDLK_s:
			currentPalette--;
			needSpriteRefresh = true;
			break;
		
		case SDLK_f:
			currentPalette++;
			needSpriteRefresh = true;
			break;
		}

		currentSprite = (currentSprite + nsprites) % nsprites;
		currentPalette = (currentPalette + npalettes) % npalettes;
	}
}


