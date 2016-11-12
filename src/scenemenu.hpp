#ifndef SCENEMENU_H
#define SCENEMENU_H

#include "scene.hpp"
#include "character.hpp"

#include "glsprite.hpp"

#include <vector>
#include <memory>

namespace Nugem {

class MenuCharacter {
public:
	MenuCharacter(Character *);
	Character &charObject();
	size_t spriteIndex;
	size_t bigSpriteIndex;
private:
	std::unique_ptr<Character> m_character;
};

class Game;

class SceneMenu: public Scene, public InputReceiver {
public:
	SceneMenu(Game &);
	~SceneMenu();
	void update();
	bool render(GlGraphics & glGraphics);
	void receiveInput(InputDevice * device, InputState &state);
	bool loading();
protected:
	void findCharacters();
	std::vector<MenuCharacter> m_characters;
	std::unique_ptr<GlSpriteCollection> m_textureAtlas;
	Game &m_game;
	size_t m_selectedCharacter;
};

}

#endif // SCENEMENU_H
