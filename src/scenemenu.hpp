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
private:
	std::unique_ptr<Character> mCharacter;
};

class Game;

class SceneMenu: public Scene, public InputReceiver {
public:
	SceneMenu(Game &);
	~SceneMenu();
	void update();
	bool render(GlGraphics & glGraphics);
	void receiveInput(InputDevice * device, InputState state);
	bool loading();
protected:
	void findCharacters();
	std::vector<MenuCharacter> mCharacters;
	std::unique_ptr<GlSpriteCollection> mTextureAtlas;
	Game &mGame;
	int selectedCharacter;
};

}

#endif // SCENEMENU_H
