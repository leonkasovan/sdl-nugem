#ifndef FIGHT_HPP
#define FIGHT_HPP

#include "../scene.hpp"
#include "../game.hpp"
#include "../input.hpp"
#include "fightcharacter.hpp"
#include "../mugen/stage.hpp"
#include <array>
#include <memory>

namespace Nugem {

class Fight: public Scene, InputReceiver
{
public:
	Fight(Game &, const std::string &);
	virtual ~Fight();
	virtual void update();
	virtual bool render(GlGraphics & glGraphics);
	virtual bool loading();
	void receiveInput(InputDevice * device, InputState &state);
private:
	std::array<std::unique_ptr<FightCharacter>, 2> m_characters;
	std::unique_ptr<Mugen::Stage> m_stage;
	Game &m_game;
};

}

#endif // FIGHT_HPP
