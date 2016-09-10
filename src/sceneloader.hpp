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

#ifndef SCENELOADER_HPP
#define SCENELOADER_HPP

#include "scene.hpp"

#include <memory>
#include <future>

namespace Nugem {

class SceneLoader: public Scene {
public:
	SceneLoader(Game& game, Scene *scene);
	void update();
	bool render(GlGraphics & glGraphics);
	bool loading();
private:
	Game &mGame;
	std::unique_ptr<Scene> mScene;
	std::future<bool> mFuture;
};

}

#endif // SCENELOADER_HPP

