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
#include <exception>

#include "spritehandler.h"
#include "mugen/mugenutils.h"

class Character
{
public:
	Character(const char* charid);
	virtual ~Character();
	virtual void render(SDL_Renderer * renderer);
	virtual void handleEvent(const SDL_Event e);
	const mugen::defcontents & getdef() const;
	const std::string & getdir() const;
protected:
	void loadCharacterDef(const char* filepath);
	std::string id;
	std::string name;
	mugen::defcontents def;
private:
	std::string directory;
	std::string definitionfilename;
	std::string spritefilename;
	std::string mugenversion;
	SDL_Texture * texture;
	int currentSprite;
	bool needSpriteRefresh;
	SpriteHandler * spriteHandler;
	uint16_t width;
	uint16_t height;
};

class CharacterException: public std::exception
{};

class CharacterSpriteError: public CharacterException
{};

#endif // CHARACTER_H

