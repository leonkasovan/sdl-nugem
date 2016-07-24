#include "scenemenu.hpp"
#include "game.hpp"

#include <iostream>
#include <dirent.h>
#include <SDL.h>

#include <SDL_image.h>
#include "fight/fight.hpp"
#include "game.hpp"

SceneMenu::SceneMenu()
{
	   m_bigFace = nullptr;
}

bool SceneMenu::render(GlGraphics & glGraphics)
{
	int ncols = 8;
	int rw = 50;
	int rh = 50;
	int margin = 20;
	for (size_t i = 0; i < m_characters.size(); i++) {
		SDL_Rect rectangle { rw * 2 * ( (int) i % ncols), rh * 2 * ( (int) i / ncols), rw, rh};
		glGraphics.render2DTexture(m_selectionfaces[i], &rectangle);
	}
	if (m_bigFace) {
		SDL_Rect bigrect { 20, 20, -1, -1 };
		glGraphics.render2DTexture(*m_bigFace, &bigrect);
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
				m_characters.push_back(Character(name));
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
	if (m_bigFace)
		delete m_bigFace;
}

bool SceneMenu::loader()
{
	findCharacters();
	for (Character & chara : m_characters) {
		chara.loadForMenu();
		m_selectionfaces.push_back(m_game->glGraphics().surfaceToTexture(chara.selectionSprite().surface()));
	}
	selectedCharacter = 0;
	if (m_bigFace)
		delete m_bigFace;
	   m_bigFace = new GlTexture(m_game->glGraphics().surfaceToTexture(m_characters.at(selectedCharacter).faceSprite().surface()));
	return true;
}

void SceneMenu::update()
{
}

void SceneMenu::receiveInput(InputDevice * device, inputstate_t state)
{
	if (state.back == INPUT_B_PRESSED) {
		m_game->requestQuit();
		return;
	}
	
	if (state.start == INPUT_B_PRESSED) {
		m_game->setScene(new Fight(new Character(m_characters.at(selectedCharacter))));
	}
	
	int value = 0;
	
	if (state.d >= 7)
		value++;
	if (state.d > 0 && state.d <= 3)
		value--;
	
	if (value) {
		selectedCharacter += m_characters.size() + value;
		selectedCharacter %= m_characters.size();
		if (m_bigFace)
			delete m_bigFace;
		      m_bigFace = new GlTexture(m_game->glGraphics().surfaceToTexture(m_characters.at(selectedCharacter).faceSprite().surface()));
	}
}



