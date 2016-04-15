#include "scenemenu.h"

#include <iostream>
#include <dirent.h>

SceneMenu::SceneMenu()
{
	findCharacters();
	for (Character & chara: characters) {
		chara.loadForMenu();
	}
}

bool SceneMenu::render(GlGraphics & glGraphics)
{
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
				characters.push_back(Character(name));
			}
			catch (CharacterLoadException & error) {
				std::cerr << "Couldn't load character " << name << ": " << error.what() << std::endl;
			}
		}
		closedir(chardir);
	}
}
