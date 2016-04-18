#include "scenefight.h"
#include "scenemenu.h"
#include "game.h"

SceneFight::SceneFight(Character * charLeft)
{
	m_charLeft = charLeft;
	m_charRight = nullptr;
}

SceneFight::~SceneFight()
{
	if (m_charLeft)
		delete m_charLeft;
	if (m_charRight)
		delete m_charRight;
}

bool SceneFight::loader()
{

}

void SceneFight::update()
{
	for (int i = 0; i < m_game->inputManager().deviceNumber(); i++) {
		InputDevice & input = m_game->inputManager().device(i);
		if (input.getState().back == INPUT_B_PRESSED)
			m_game->setScene(new SceneMenu());
	}
}

bool SceneFight::render(GlGraphics & glGraphics)
{

}
