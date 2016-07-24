#ifndef SCENE_H
#define SCENE_H

#include "glgraphics.hpp"

class Scene
{
public:
	virtual ~Scene() {};
	virtual void update() = 0;
	virtual bool render(GlGraphics & glGraphics) = 0;
	void load(Game & game);
	bool loaded() { return m_loaded; };
protected:
	virtual bool loader() = 0;
	bool m_loaded = false;
	Game * m_game = nullptr;
};

#endif // SCENE_H

