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
	currentPalette = 0;
	currentAnimStep = 0;
	spriteHandler = nullptr;
	directory = "chars/" + id;
	definitionfilename = id + ".def";
	loadCharacterDef((directory + "/" + definitionfilename).c_str());
	std::string cmdfile = (std::string) def["Files"]["cmd"];
	loadCharacterCmd((directory + "/" + cmdfile).c_str());
	std::string airfile = (std::string) def["Files"]["anim"];
	loadCharacterAnimations((directory + "/" + airfile).c_str());
	curAnimIterator = animations.begin();
}

Character::Character(Character && character)
{
	// Initialization
	texture = nullptr;
	
	// Move
	std::swap(id, character.id);
	std::swap(name, character.name);
	std::swap(x, character.x);
	std::swap(y, character.y);
	std::swap(currentPalette, character.currentPalette);
	std::swap(currentAnimStep, character.currentAnimStep);
	std::swap(directory, character.directory);
	std::swap(definitionfilename, character.definitionfilename);
	std::swap(definitionfilename, character.definitionfilename);
	std::swap(spritefilename, character.spritefilename);
	std::swap(spriteHandler, character.spriteHandler);
	std::swap(animations, character.animations);
	std::swap(texture, character.texture);
	std::swap(def, character.def);
	curAnimIterator = animations.begin();
	
	// Removing references from the previous object
	character.spriteHandler = nullptr;
	character.texture = nullptr;
}

Character::~Character()
{
	if (spriteHandler)
		delete spriteHandler;
	if (texture)
		SDL_DestroyTexture(texture);
}

mugen::DefinitionFile & Character::getdef()
{
	return def;
}

const std::__cxx11::string & Character::getdir() const
{
	return directory;
}

SpriteHandler * Character::getSpriteHandler()
{
	return spriteHandler;
}

void Character::loadCharacterDef(const char * filepath)
{
	if (spriteHandler)
		delete spriteHandler;
	def = mugen::DefinitionFile(filepath);
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
			throw CharacterLoadException(std::string("Invalid sprite file: ") + spritepath);
		}
		version = extract_version(spritefile);
		spritefile.close();
	}
	// Choosing whether to use Sffv1 or Sffv2
	if (version[0] < 2)
		spriteHandler = new mugen::Sffv1(*this, spritepath.c_str());
	else if (version[0] == 2)
		spriteHandler = new mugen::Sffv2(spritepath.c_str());
}

void Character::loadCharacterCmd(const char * filepath)
{
	//cmd = mugen::CharacterCommands(filepath);
}

void Character::loadCharacterAnimations(const char * filepath)
{
	animations = mugen::AnimationData(filepath);
}

void Character::render()
{
	int h, w;
	//SDL_GetRendererOutputSize(renderer, &w, &h);
	mugen::animation_t & animation = curAnimIterator->second;
	mugen::animstep_t & animstep = animation.steps[currentAnimStep];
	spriteHandler->setSprite(animstep.group, animstep.image);
	currentGameTick++;
	if (currentGameTick >= animstep.ticks) {
		currentAnimStep++;
		currentGameTick = 0;
	}
	if (currentAnimStep >= animation.steps.size()) {
		currentAnimStep = animation.loopstart;
	} 
	if (texture)
		SDL_DestroyTexture(texture);
	spriteHandler->setPalette(currentPalette);
	SDL_Surface * surface = spriteHandler->getSurface();
	width = surface->w;
	height = surface->h;
	//texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	SDL_Rect DestR;
	// Centering the sprite in the middle of the screen
	x = w / 2;
	y = h / 2;
	x -= spriteHandler->getImageXAxis() + animstep.x;
	y -= spriteHandler->getImageYAxis() + animstep.y;
	DestR.x = x;
	DestR.y = y;
	DestR.w = width;
	DestR.h = height;
	SDL_RendererFlip flip = (SDL_RendererFlip) 0;
	if (animstep.hinvert)
		flip = (SDL_RendererFlip) ( SDL_FLIP_HORIZONTAL | flip );
	if (animstep.vinvert)
		flip = (SDL_RendererFlip) ( SDL_FLIP_VERTICAL | flip );
	//SDL_RenderCopyEx(renderer, texture, nullptr, &DestR, 0, nullptr, flip);
}

void Character::handleEvent(const SDL_Event e)
{
	const size_t npalettes = spriteHandler->getTotalPaletteNumber();
	if (e.type == SDL_KEYDOWN) {
		// Select surfaces based on key press
		
		// For now...
		switch (e.key.keysym.sym) {
		// Changing sprites
		case SDLK_UP:
			curAnimIterator++;
			currentAnimStep = 0;
			currentGameTick = 0;
			break;

		case SDLK_DOWN:
			if (curAnimIterator == animations.begin())
				curAnimIterator = animations.end();
			curAnimIterator--;
			currentAnimStep = 0;
			currentGameTick = 0;
			break;
			
		
		case SDLK_s:
			currentPalette--;
			break;
		
		case SDLK_f:
			currentPalette++;
			break;
		}

		currentPalette = (currentPalette + npalettes) % npalettes;
		if (curAnimIterator == animations.end())
			curAnimIterator = animations.begin();
	}
}


