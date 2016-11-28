/*
 * Copyright (c) 2016 Victor Nivet
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

#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>
#include <vector>
#include <SDL.h>
#include <unordered_map>
#include <exception>

#include "mugen/air.hpp"
#include "mugen/cmd.hpp"
#include "mugen/sprites.hpp"
#include "mugen/def.hpp"

namespace Nugem {

class CharacterLoadException: public std::runtime_error {
public:
    explicit CharacterLoadException(const std::string& __arg): std::runtime_error(__arg) {};
    explicit CharacterLoadException(): std::runtime_error("Error loading character") {};
};

class Character
{
public:
    Character(const char* charid);
    Character(const Character & character);
    Character(Character&& character);
    virtual ~Character();
    virtual void handleEvent(const SDL_Event e);
    Mugen::DefinitionFile& def();
    const std::string & id() const;
    const std::string & name() const;
    const std::string & dir() const;
	Mugen::SpriteLoader & spriteLoader();
protected:
    void loadCharacterDef(const char* filepath);
    void loadCharacterAnimations(const char* filepath);
    void loadCharacterCmd(const char* filepath);
    std::string m_id;
    std::string m_name;
    Mugen::DefinitionFile m_def;
    Mugen::AnimationData m_animations;
    Mugen::CharacterCommands m_cmd;
    Mugen::SpriteLoader m_spriteLoader;
    unsigned int m_x;
    unsigned int m_y;
    std::vector< std::unordered_map< Mugen::Spriteref, Mugen::Sprite > > m_sprites;
    std::vector<Mugen::Sprite> m_selectionSprite;
    std::vector<Mugen::Sprite> m_faceSprite;
private:
    std::string m_directory;
    std::string m_definitionFilename;
    std::string m_spriteFilename;
    std::string m_mugenVersion;
    size_t m_currentPalette;
    size_t m_currentAnimStep;
    size_t m_currentGameTick;
    Mugen::Spriteref mCurrentSprite;
};

class CharacterException: public std::exception
{};

class CharacterSpriteError: public CharacterException
{};

}

#endif // CHARACTER_HPP

