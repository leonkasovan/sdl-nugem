#include "scene.h"

void Scene::load(Game * game)
{
	m_game = game;
	m_loaded = _load();
}
