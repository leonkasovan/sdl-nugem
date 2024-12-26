#include "scenemenu.hpp"
#include "game.hpp"

#include <iostream>
#include <dirent.h>
#include <SDL.h>

#include <SDL_image.h>
#include "fight/fight.hpp"
#include "game.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

using namespace Nugem;

SceneMenu::SceneMenu(Game &game): m_game(game)
{
	   m_game.inputManager().addReceiver(this);
}

SceneMenu::~SceneMenu()
{
	   m_game.inputManager().removeReceiver(this);
}

bool SceneMenu::render(GlGraphics & glGraphics)
{
	if (m_textureAtlas) {
		GlSpriteDisplayer spriteDisplay(*(m_textureAtlas.get()));
		{
			int squareside = 50;
			SDL_Rect location{ 100, 100, squareside, squareside};
			for (size_t i = 0; i < m_characters.size(); i ++) {
				spriteDisplay.addSprite(m_characters[i].spriteIndex, location);
				location.y += squareside + 10;
			}
		}
		{
			SDL_Rect bigLoc { 350, 100, 250, 250};
			spriteDisplay.addSprite(m_characters[m_selectedCharacter].bigSpriteIndex, bigLoc);
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
				m_characters.emplace_back(new Character(name));
			}
			catch
				(CharacterLoadException & error) {
				std::cerr << "Couldn't load character " << name << ": " << error.what() << std::endl;
			}
		}
		closedir(chardir);
	}
}

bool SceneMenu::loading()
{
	findCharacters();
	GlSpriteCollectionBuilder textureAtlasBuilder;
	std::vector<Mugen::Spriteref> menurefs { Mugen::Spriteref(9000, 0), Mugen::Spriteref(9000, 1) };
	for (auto & chara : m_characters) {
		auto menusprites = chara.charObject().spriteLoader().load(menurefs.begin(), menurefs.end());
		if (!menusprites.empty() && menusprites[0].count(menurefs[0]) && menusprites[0].count(menurefs[1])) {
			chara.spriteIndex = textureAtlasBuilder.addSprite(menusprites[0].at(menurefs[0]).surface());
			chara.bigSpriteIndex = textureAtlasBuilder.addSprite(menusprites[0].at(menurefs[1]).surface());
		}
	}
	m_textureAtlas.reset(textureAtlasBuilder.build());
	m_selectedCharacter = 0;
	return true;
}

void SceneMenu::update()
{
}

void SceneMenu::receiveInput(InputDevice *, InputState &state)
{
	if (state.back == INPUT_B_PRESSED) {
		m_game.requestQuit();
	}
	if (state.start == INPUT_B_PRESSED) {
		m_game.changeScene(new Fight(m_game, m_characters[m_selectedCharacter].charObject().id()));
	}
	if (state.d == INPUT_D_S) {
		m_selectedCharacter += m_characters.size() - 1;
		m_selectedCharacter %= m_characters.size();
	}
	if (state.d == INPUT_D_N) {
		m_selectedCharacter++;
		m_selectedCharacter %= m_characters.size();
	}
}

Nugem::MenuCharacter::MenuCharacter(Nugem::Character *character): m_character(character)
{
}

Nugem::Character & Nugem::MenuCharacter::charObject()
{
	return *(m_character.get());
}




