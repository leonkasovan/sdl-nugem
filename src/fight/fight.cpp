#include "fight.hpp"
#include "../scenemenu.hpp"
#include "../game.hpp"

namespace Nugem {

Fight::Fight(Game &game, const std::string &characterName): m_game(game)
{
	m_game.inputManager().addReceiver(this);
	m_characters[0].reset(new FightCharacter(new Character(characterName.c_str()), m_game.inputManager().device(0)));
	m_stage.reset(new Mugen::Stage("kim"));
}

Fight::~Fight()
{
	m_game.inputManager().removeReceiver(this);
}

bool Fight::loading()
{
	m_stage->initialize();
	return true;
}

void Fight::update()
{
	/*
    for (int i = 0; i < m_game->inputManager().deviceNumber(); i++) {
        InputDevice & input = m_game->inputManager().device(i);
        if (input.getState().back == INPUT_B_PRESSED)
            m_game->setScene(new SceneMenu());
    }
    */
}

bool Fight::render(GlGraphics & glGraphics)
{
	m_stage->renderBackground(glGraphics);
    return true;
}

void Fight::receiveInput(InputDevice * device, InputState &state)
{
	if (state.back == INPUT_B_PRESSED) {
		m_game.changeScene(new SceneMenu(m_game));
	}
}

}
