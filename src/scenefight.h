#ifndef SCENE_FIGHT_H
#define SCENE_FIGHT_H

#include "scene.h"
#include "character.h"

class SceneFight: public Scene
{
protected:
	Character* charLeft;
	Character* charRight;
};

#endif // SCENE_FIGHT_H
