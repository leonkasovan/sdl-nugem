#ifndef SCENE_H
#define SCENE_H

#include "glgraphics.h"

class Scene
{
public:
	virtual ~Scene() {};
	virtual bool render(GlGraphics & glGraphics) = 0;
};

#endif // SCENE_H

