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

#include "mugen/air.hpp"
#include "mugen/cmd.hpp"
#include "mugen/sprites.hpp"
#include "mugen/def.hpp"

class CharacterLoadException: public std::runtime_error {
public:
explicit CharacterLoadException(const std::string& __arg): std::runtime_error(__arg) {};
explicit CharacterLoadException(): std::runtime_error("Error loading character") {};
};

namespace mugen {
class DefinitionFile;
class AnimationData;
class CharacterCommands;
class SpriteLoader;
}

class Character
{
public:
	Character(const char* charid);
	Character(const Character & character);
	Character(Character&& character);
	virtual ~Character();
	virtual void handleEvent(const SDL_Event e);
	mugen::DefinitionFile& def();
	const std::string & id() const;
	const std::string & name() const;
	const std::string & dir() const;
	const mugen::Sprite & currentSprite() const;
	const mugen::Sprite & selectionSprite() const;
	const mugen::Sprite & faceSprite() const;
	void loadForMenu();
protected:
	void loadCharacterDef(const char* filepath);
	void loadCharacterAnimations(const char* filepath);
	void loadCharacterCmd(const char* filepath);
	std::string m_id;
	std::string m_name;
	mugen::DefinitionFile m_def;
	mugen::AnimationData m_animations;
	mugen::CharacterCommands m_cmd;
	mugen::SpriteLoader m_spriteLoader;
	unsigned int m_x;
	unsigned int m_y;
	std::vector< std::unordered_map< mugen::spriteref, mugen::Sprite > > m_sprites;
	std::vector<mugen::Sprite> m_selectionSprite;
	std::vector<mugen::Sprite> m_faceSprite;
private:
	std::string m_directory;
	std::string m_definitionfilename;
	std::string m_spritefilename;
	std::string m_mugenversion;
	size_t m_currentPalette;
	size_t m_currentAnimStep;
	size_t m_currentGameTick;
	mugen::spriteref m_currentSprite;
};

class CharacterException: public std::exception
{};

class CharacterSpriteError: public CharacterException
{};

#endif // CHARACTER_H

