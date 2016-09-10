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

#include "game.hpp"
#include "character.hpp"
#include "scene.hpp"
#include "player.hpp"

#include "scenemenu.hpp"

#include <iostream>

namespace Nugem {

Game::Game(): mGlGraphics(mWindow)
{
	if (mWindow)
		mContinueMainLoop = true;
	else
		std::cerr << "Failed to open a window" << std::endl;
}

Game::~Game()
{
	// SDL deinitialization
	mGlGraphics.finish();
}

void Game::update()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		mInputManager.processSDLEvent(e);
	}
	
	mGlGraphics.clear();
	// update
	if (mCurrentScene) {
		mCurrentScene->update();
		mCurrentScene->render(mGlGraphics);
	}
	mGlGraphics.display();
	
	if (mNextScene) {
		mCurrentScene.reset(nullptr);
		mNextScene.swap(mCurrentScene);
	}
}

void Game::run()
{
	mInputManager.initialize(this);
	changeScene(new SceneMenu(*this));
	mGlGraphics.initialize(this);
	// 60 fps
	uint32_t tickdelay = 1000 / 60;
	// Main game loop
	while (mContinueMainLoop && !SDL_QuitRequested()) {
		uint32_t tick = SDL_GetTicks();
		update();
		uint32_t dt = SDL_GetTicks() - tick;
		if (dt < tickdelay)
			SDL_Delay(tickdelay - dt);
	}
}

bool Game::requestQuit()
{
	   mContinueMainLoop = false;
	return true;
}

InputManager & Game::inputManager()
{
	return mInputManager;
}

Scene &Game::currentScene()
{
	return *mCurrentScene;
}

void Game::changeScene(Scene *newScene)
{
	mNextScene.reset(newScene);
}


}
