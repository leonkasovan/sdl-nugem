/*
 * Copyright (c) 2016 Victor Nivet
 * 
 * This file is part of Nugem.
 * 
 * Nugem is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 * Nugem is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 *  along with Nugem.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include "input.hpp"
#include "character.hpp"
#include "window.hpp"
#include "glgraphics.hpp"

namespace Nugem {

class Scene;
class Player;

class Game
{
public:
	Game();
	~Game();
	void run();
	GlGraphics & glGraphics() { return mGlGraphics; };
	Scene &currentScene();
	InputManager & inputManager();
	void changeScene(Scene *newScene);
	auto &players() { return mPlayers; };
	bool requestQuit();
protected:
	void update();
	InputManager mInputManager;
private:
	Window mWindow;
	GlGraphics mGlGraphics;
	std::unique_ptr<Scene> mCurrentScene;
	std::unique_ptr<Scene> mNextScene;
	std::vector<std::unique_ptr<Player>> mPlayers;
	bool mContinueMainLoop;
};

}

#endif // GAME_H

