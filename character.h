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

