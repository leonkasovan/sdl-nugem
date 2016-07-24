#ifndef SCENE_FIGHT_H
#define SCENE_FIGHT_H

#include "../scene.h"
#include "../character.h"

class FightCharacter
{
  
};

class Fight: public Scene
{
public:
	Fight(Character * charLeft);
	virtual ~Fight();
	virtual void update();
	virtual bool render(GlGraphics & glGraphics);
protected:
	virtual bool loader();
	Character * m_charLeft;
};

#endif // SCENE_FIGHT_H
