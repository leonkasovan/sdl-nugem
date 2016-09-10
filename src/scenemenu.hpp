#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "scene.hpp"
#include "character.hpp"

#include <vector>

namespace Nugem {

class Game;

class SceneMenu: public Scene {
public:
	SceneMenu(Game &);
	~SceneMenu();
	void update();
	bool render(GlGraphics & glGraphics);
	void receiveInput(InputDevice * device, InputState state);
	bool loading();
protected:
	void findCharacters();
	std::vector<Character> m_characters;
	std::vector<GlTexture> m_selectionfaces;
	Game &mGame;
	GlTexture * m_bigFace;
	int selectedCharacter;
};

}

#endif // SCENEMENU_H
