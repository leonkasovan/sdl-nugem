#include "fight.hpp"
#include "../scenemenu.hpp"
#include "../game.hpp"

namespace Nugem {

Fight::Fight(Character *)
{
}

Fight::~Fight()
{
}

bool Fight::loading()
{
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
    return true;
}

}
