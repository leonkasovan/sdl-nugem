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

#include "sceneloader.hpp"
#include "scenemenu.hpp"

#include <iostream>

namespace Nugem {

Game::Game(): m_glGraphics(m_window), mEventHandler(*this)
{
    if (m_window)
        m_continueMainLoop = true;
    else
        std::cerr << "Failed to open a window" << std::endl;
}

Game::~Game()
{
    // SDL deinitialization
    m_glGraphics.finish();
}

void Game::update()
{
    mEventHandler.handleSDLEvents();

    m_glGraphics.clear();
    // update
    if (m_currentScene) {
        m_currentScene->update();
        m_currentScene->render(m_glGraphics);
    }
    m_glGraphics.display();
}

void Game::run()
{
    m_inputManager.initialize(this);
    changeScene(new SceneMenu(*this));
    m_glGraphics.initialize(this);
	m_window.raise();
    // 60 fps
    uint32_t tickdelay = 1000 / 60;
    // Main game loop
    while (m_continueMainLoop && !SDL_QuitRequested()) {
        uint32_t tick = SDL_GetTicks();
        update();
        uint32_t dt = SDL_GetTicks() - tick;
        if (dt < tickdelay)
            SDL_Delay(tickdelay - dt);
    }
}

bool Game::requestQuit()
{
    m_continueMainLoop = false;
    return true;
}

InputManager & Game::inputManager()
{
    return m_inputManager;
}

Window &Game::window()
{
    return m_window;
}

Scene &Game::currentScene()
{
    return *m_currentScene;
}

void Game::changeScene(Scene *newScene)
{
    m_currentScene.reset(new SceneLoader(*this, newScene));
}

void Game::loadedScene(Scene *loadedScene)
{
    m_currentScene.reset(loadedScene);
}


}
