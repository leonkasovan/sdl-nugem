#include "scenemenu.h"
#include "game.h"

#include <iostream>
#include <dirent.h>
#include <SDL.h>

#include <SDL_image.h>

SceneMenu::SceneMenu()
{
}

bool SceneMenu::render(GlGraphics & glGraphics)
{
	int ncols = 8;
	int rw = 50;
	int rh = 50;
	for (int i = 0; i < m_characters.size(); i++) {
		SDL_Rect rectangle{ rw * 2 * (i % ncols), rh * 2 * (i / ncols), rw, rh};
		glGraphics.render2DTexture(m_faces[i], &rectangle);
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
			catch (CharacterLoadException & error) {
				std::cerr << "Couldn't load character " << name << ": " << error.what() << std::endl;
			}
		}
		closedir(chardir);
	}
}

SceneMenu::~SceneMenu()
{
	for (GlTexture face: m_faces) {
		if (face.tid)
			glDeleteTextures(1, &face.tid);
	}
}

bool SceneMenu::_load()
{
	findCharacters();
	for (Character & chara: m_characters) {
		chara.loadForMenu();
		m_faces.push_back(m_game->glGraphics().surfaceToTexture(chara.selectionSprite().surface()));
	}
	return true;
}

