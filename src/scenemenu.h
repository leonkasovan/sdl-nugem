#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "scene.h"
#include "character.h"

#include <vector>

class SceneMenu: public Scene {
public:
	SceneMenu();
	~SceneMenu();
	bool render(GlGraphics & glGraphics);
protected:
	virtual bool _load();
	void findCharacters();
	std::vector<Character> m_characters;
	std::vector<GlTexture> m_faces;
};

#endif // SCENEMENU_H
