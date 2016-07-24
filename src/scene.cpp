#include "scene.hpp"

void Scene::load(Game & game)
{
	m_game = &game;
	m_loaded = loader();
}
