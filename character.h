#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include <vector>
#include <SDL.h>

#include "spritehandler.h"

class Character
{
public:
	Character(const char* charid);
	virtual ~Character();
	virtual void render(SDL_Renderer * renderer);
	virtual void handleEvent(const SDL_Event e);
protected:
	std::string id;
	std::string name;
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

#endif // CHARACTER_H

