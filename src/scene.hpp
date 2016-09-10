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

#ifndef SCENE_HPP
#define SCENE_HPP

#include "glgraphics.hpp"

namespace Nugem {

class Scene {
public:
	virtual ~Scene() {};
	virtual void update() = 0;
	virtual bool render(GlGraphics & glGraphics) = 0;
	virtual bool loading() = 0;
};

}

#endif // SCENE_HPP

