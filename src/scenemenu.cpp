#include "scenemenu.hpp"
#include "game.hpp"

#include <iostream>
#include <dirent.h>
#include <SDL.h>

#include <SDL_image.h>
#include "fight/fight.hpp"
#include "game.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace Nugem;

SceneMenu::SceneMenu(Game &game): mGame(game)
{
}

bool SceneMenu::render(GlGraphics & glGraphics)
{
	if (mTextureAtlas) {
		GlSpriteDisplayer spriteDisplay(*(mTextureAtlas.get()));
		{
			int squareside = 50;
			SDL_Rect location{ 100, 100, squareside, squareside};
			for (size_t i = 0; i < mTextureAtlas->size(); i += 2) {
				spriteDisplay.addSprite(i, location);
				location.y += squareside + 10;
			}
		}
		{
			SDL_Rect bigLoc { 350, 100, 250, 250};
			spriteDisplay.addSprite(m_selectedCharacter + 1, bigLoc);
		}
		spriteDisplay.display(glGraphics);
	}
	return true;
}

void SceneMenu::findCharacters()
{
	DIR * chardir = nullptr;
	struct dirent * chardirent = nullptr;
	chardir = opendir("chars");
	if (chardir != nullptr) {
		while ((chardirent = readdir(chardir))) {
			const char * name = chardirent->d_name;
			if (name[0] == '.')
				continue;
			try {
				mCharacters.emplace_back(new Character(name));
			}
			catch
				(CharacterLoadException & error) {
				std::cerr << "Couldn't load character " << name << ": " << error.what() << std::endl;
			}
		}
		closedir(chardir);
	}
}

SceneMenu::~SceneMenu()
{
}

bool SceneMenu::loading()
{
	findCharacters();
	GlSpriteCollectionBuilder textureAtlasBuilder;
	std::vector<Mugen::Spriteref> menurefs { Mugen::Spriteref(9000, 0), Mugen::Spriteref(9000, 1) };
	for (auto & chara : mCharacters) {
		auto menusprites = chara.charObject().spriteLoader().load(menurefs.begin(), menurefs.end());
		textureAtlasBuilder.addSprite(menusprites[0].at(Mugen::Spriteref(9000, 0)).surface());
		textureAtlasBuilder.addSprite(menusprites[0].at(Mugen::Spriteref(9000, 1)).surface());
	}
	mTextureAtlas.reset(textureAtlasBuilder.build());
	return true;
}

void SceneMenu::update()
{
}

void SceneMenu::receiveInput(InputDevice * device, InputState state)
{
}

Nugem::MenuCharacter::MenuCharacter(Nugem::Character *character): mCharacter(character)
{
}

Nugem::Character & Nugem::MenuCharacter::charObject()
{
	return *(mCharacter.get());
}



