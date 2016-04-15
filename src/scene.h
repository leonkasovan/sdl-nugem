#ifndef SCENE_H
#define SCENE_H

#include "glgraphics.h"

class Scene
{
public:
	virtual ~Scene() {};
	virtual bool render(GlGraphics & glGraphics) = 0;
	void load(Game * game);
	bool loaded() { return m_loaded; };
protected:
	virtual bool _load() = 0;
	bool m_loaded = false;
	Game * m_game = nullptr;
};

#endif // SCENE_H

