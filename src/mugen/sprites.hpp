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

#ifndef MUGEN_SPRITES_H
#define MUGEN_SPRITES_H

#include <SDL.h>
#include <array>
#include <unordered_map>
#include <vector>
#include <fstream>

namespace Nugem {
namespace Mugen {
	
struct Spriteref {
	int group;
	int image;
	Spriteref(): group(0), image(0) {}
	Spriteref(int g, int i): group(g), image(i) {}
	bool operator==(const Spriteref & ref) const { return group == ref.group && image == ref.image; }
};

}
}

namespace std {

  template <>
  struct hash<Nugem::Mugen::Spriteref>
  {
    std::size_t operator()(const Nugem::Mugen::Spriteref & k) const
    {
      using std::size_t;
      using std::hash;
      using std::string;

      return (hash<int>()(k.group)) ^ (hash<int>()(k.image) << 1);
    }
  };

}

namespace Nugem {

class Character;

namespace Mugen {
	
class Sprite {
public:
	Sprite(Spriteref reference, SDL_Surface * surface);
	Sprite(Spriteref reference, SDL_Surface * surface, int palette);
	virtual ~Sprite();
	Sprite(const Sprite & originalSprite);
	Sprite(Sprite && originalSprite);
	Sprite & operator=(const Sprite & originalSprite);
	Sprite & operator=(Sprite && originalSprite);
	const Spriteref ref() const { return m_ref; };
	const SDL_Surface * surface() const { return m_surface; };
	const int palette() const { return m_npalette; };
protected:
	Spriteref m_ref;
	int m_npalette;
	SDL_Surface * m_surface;
	static SDL_Surface * copySurface(SDL_Surface * surface);
};

class SpriteHandler {
public:
	virtual ~SpriteHandler() {};
	virtual void load() = 0;
	virtual void load(std::vector<Spriteref>::iterator first, std::vector<Spriteref>::iterator last) = 0;
	std::vector<std::unordered_map<Spriteref, Sprite>> sprites() { return m_sprites; };
	
protected:
	std::vector<std::unordered_map<Spriteref, Sprite>> m_sprites;
};

// Function for both SFFv1 and SFFv2 sprites
std::array<uint8_t, 4> extract_version(std::ifstream & fileobj);

// Little endian to big endian
uint32_t read_uint32(std::ifstream & fileobj);

uint16_t read_uint16(std::ifstream & fileobj);

class SpriteLoader {
public:
	SpriteLoader();
	void initialize(const std::string & sffpath, Character * character);
	std::vector<std::unordered_map<Spriteref, Sprite>> load();
	std::vector<std::unordered_map<Spriteref, Sprite>> load(std::vector<Spriteref>::iterator first, std::vector<Spriteref>::iterator last);
	std::unordered_map<Spriteref, Sprite> loadForPalette(int palette);
	bool isInitialized() const;
protected:
	SpriteHandler * createHandler();
	std::string m_sffFile;
	std::array<uint8_t, 4> m_sffVersion;
	Character * m_character;
};

}
}

#endif // MUGEN_SPRITES_H
