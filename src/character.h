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

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
#include <SDL.h>
#include <unordered_map>
#include <exception>

#include "spritehandler.h"
#include "mugen/mugenutils.h"
#include "mugen/def.h"
#include "mugen/air.h"
#include "mugen/cmd.h"

class CharacterLoadException: public std::runtime_error {
public:
explicit CharacterLoadException(const std::string& __arg): std::runtime_error(__arg) {};
explicit CharacterLoadException(): std::runtime_error("Error loading character") {};
};

class Character
{
public:
	Character(const char* charid);
	Character(Character&& character);
	virtual ~Character();
	virtual void render(SDL_Renderer * renderer);
	virtual void handleEvent(const SDL_Event e);
	mugen::DefinitionFile& getdef();
	const std::string & getdir() const;
	SpriteHandler * getSpriteHandler();
protected:
	void loadCharacterDef(const char* filepath);
	void loadCharacterAnimations(const char* filepath);
	void loadCharacterCmd(const char* filepath);
	std::string id;
	std::string name;
	mugen::DefinitionFile def;
	mugen::AnimationData animations;
	mugen::CharacterCommands cmd;
	unsigned int x;
	unsigned int y;
private:
	std::string directory;
	std::string definitionfilename;
	std::string spritefilename;
	std::string mugenversion;
	SDL_Texture * texture;
	size_t currentPalette;
	mugen::AnimationData::iterator curAnimIterator;
	size_t currentAnimStep;
	size_t currentGameTick;
	SpriteHandler * spriteHandler;
	uint16_t width;
	uint16_t height;
};

class CharacterException: public std::exception
{};

class CharacterSpriteError: public CharacterException
{};

#endif // CHARACTER_H

