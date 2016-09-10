#ifndef FIGHT_HPP
#define FIGHT_HPP

#include "../scene.hpp"
#include "fightcharacter.hpp"
#include <array>
#include <memory>

namespace Nugem {

class Fight: public Scene
{
public:
	Fight(Character * charLeft);
	virtual ~Fight();
	virtual void update();
	virtual bool render(GlGraphics & glGraphics);
	virtual bool loading();
private:
	std::array<std::unique_ptr<FightCharacter>, 2> mCharacters;
};

}

#endif // FIGHT_HPP
