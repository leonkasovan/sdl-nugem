#include "fight.h"
#include "../scenemenu.h"
#include "../game.h"

Fight::Fight(Character * charLeft)
{
    m_charLeft = charLeft;
}

Fight::~Fight()
{
    if (m_charLeft)
        delete m_charLeft;
}

bool Fight::loader()
{
	return true;
}

void Fight::update()
{
    for (int i = 0; i < m_game->inputManager().deviceNumber(); i++) {
        InputDevice & input = m_game->inputManager().device(i);
        if (input.getState().back == INPUT_B_PRESSED)
            m_game->setScene(new SceneMenu());
    }
}

bool Fight::render(GlGraphics & glGraphics)
{
    return true;
}
