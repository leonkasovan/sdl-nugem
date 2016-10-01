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

#include "sceneloader.hpp"
#include <iostream>
#include "game.hpp"

namespace Nugem {

SceneLoader::SceneLoader(Game &game, Scene *scene): mGame(game), mScene(scene)
{
// 	resetFuture();
	mScene->loading();
}

void SceneLoader::update()
{
	mScene->update();
	mGame.loadedScene(mScene.release());
// 	if (mFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
// 		if (!mFuture.get()) {
// 			resetFuture();
// 			return;
// 		}
// 		mScene->update();
// 		mGame.loadedScene(mScene.release());
// 	}
}

void SceneLoader::resetFuture()
{
// 	mFuture = std::async(std::launch::async, [&]() { return mScene->loading(); });
// 	mFuture = std::async(std::launch::deferred, [&]() { return mScene->loading(); });
// 	mFuture.wait();
}

bool SceneLoader::render(GlGraphics &)
{
	return true;
}

bool SceneLoader::loading()
{
	return true;
}

}
