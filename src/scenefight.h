#ifndef SCENE_FIGHT_H
#define SCENE_FIGHT_H

#include "scene.h"
#include "character.h"

class SceneFight: public Scene
{
public:
	SceneFight(Character * charLeft);
	virtual ~SceneFight();
	virtual void update();
	virtual bool render(GlGraphics & glGraphics);
protected:
	virtual bool loader();
	Character * m_charLeft;
	Character * m_charRight;
};

#endif // SCENE_FIGHT_H
