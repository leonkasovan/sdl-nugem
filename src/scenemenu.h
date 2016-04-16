#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "scene.h"
#include "character.h"

#include <vector>

class Game;

class SceneMenu: public Scene {
public:
	SceneMenu();
	~SceneMenu();
	void update();
	bool render(GlGraphics & glGraphics);
	void receiveInput(InputDevice * device, inputstate_t state);
protected:
	virtual bool loader();
	void findCharacters();
	std::vector<Character> m_characters;
	std::vector<GlTexture> m_selectionfaces;
	GlTexture * m_bigface;
	int selectedCharacter;
};

#endif // SCENEMENU_H
