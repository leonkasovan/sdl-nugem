#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "scene.h"
#include "character.h"

#include <vector>

class SceneMenu: public Scene {
public:
	SceneMenu();
	bool render(GlGraphics & glGraphics);
protected:
	void findCharacters();
	std::vector<Character> characters;
};

#endif // SCENEMENU_H
