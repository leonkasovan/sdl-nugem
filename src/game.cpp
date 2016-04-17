/*
 * Copyright (C) Victor Nivet
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

#include "game.h"
#include "character.h"

#include "scenemenu.h"

#include <iostream>

Game::Game(): m_currentScene(nullptr), m_nextScene(nullptr)
{
	// SDL initialization
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
	// Initialize window
	m_window = SDL_CreateWindow("NUGEM",
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
	                          SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
	if (m_window)
		m_continueMainLoop = true;
	else
		std::cerr << "Failed to open a window" << std::endl;
}

Game::~Game()
{
	if (m_currentScene)
		delete m_currentScene;
	if (m_nextScene)
		delete m_nextScene;
	// SDL deinitialization
	m_glGraphics.finish();
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Game::update()
{
	SDL_Event e;
	if (!m_currentScene->loaded())
		m_currentScene->load(*this);
	while (SDL_PollEvent(&e) != 0) {
		m_inputManager.processSDLEvent(e);
	}
	// update
	if (m_currentScene->loaded())
		m_currentScene->update();
	
	m_glGraphics.clear();
	//Place your simulation code and rendering code here
	if (m_currentScene)
		m_currentScene->render(m_glGraphics);
	m_glGraphics.display();
	
	if (m_nextScene) {
		if (m_currentScene)
			delete m_currentScene;
		m_currentScene = m_nextScene;
		m_nextScene = nullptr;
	}
}

void Game::run()
{
	m_inputManager.initialize(this);
	m_currentScene = new SceneMenu();
	m_glGraphics.initialize(this, m_window);
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

Scene * Game::currentScene()
{
	return m_currentScene;
}

void Game::setScene(Scene * newScene)
{
	if (m_nextScene) {
		delete m_nextScene;
		m_nextScene = nullptr;
	}
	m_nextScene = newScene;
}

std::vector< Player * > Game::players()
{
	return m_players;
}

