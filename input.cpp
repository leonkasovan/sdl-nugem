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

#include "input.h"

#include <stdio.h>
#include <iostream>

#include "player.h"

const char * InputManager::controllerDBfilename = "gamecontrollerdb.txt";

InputManager::InputManager()
{
	SDL_JoystickEventState(SDL_ENABLE);
	devices.push_back(new KeyboardInput());
	loadGameControllerDB();
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i))
			devices.push_back(new GameController(i));
		else
			devices.push_back(new Joystick(i));
	}
}

InputManager::~InputManager()
{
	for (size_t i = 0; i < devices.size(); i++)
		delete devices[i];
}

bool InputManager::loadGameControllerDB()
{
	// check if file exists
	FILE * test = fopen(controllerDBfilename, "r");
	if (test)
		fclose(test);
	else
		return false;
	// if so, load it
	SDL_GameControllerAddMappingsFromFile(controllerDBfilename);
	return true;
}

void InputManager::processSDLEvent(const SDL_Event & e)
{
	// TODO
	switch (e.type) {
		case SDL_CONTROLLERDEVICEADDED:
			return;
		case SDL_CONTROLLERDEVICEREMOVED:
			return;
		case SDL_CONTROLLERDEVICEREMAPPED:
			return;
		case SDL_JOYDEVICEADDED:
			return;
		case SDL_JOYDEVICEREMOVED:
			return;
	};
	for (unsigned int i = 0; i < devices.size(); i++)
		devices[i]->processEvent(e);
}

const InputDevice & InputManager::getDevice(size_t n) const
{
	return *devices[n];
}

const size_t InputManager::getDeviceNumber() const
{
	return devices.size();
}

const inputstate_t InputDevice::getState() const
{
	return currentState;
}

InputDevice::InputDevice()
{
	player = nullptr;
}

bool InputDevice::hasPlayerAssigned() const
{
	return (player != nullptr);
}

inputstate_t InputDevice::getState()
{
	return currentState;
}

void InputDevice::setState(inputstate_t state)
{
	currentState = state;
}

void InputDevice::assignToPlayer(Player * assignedPlayer)
{
	player = assignedPlayer;
}

void KeyboardInput::processEvent(const SDL_Event & e)
{
	switch (e.type) {
		case SDL_KEYDOWN:
// 		case SDL_KEYUP:
			const SDL_KeyboardEvent & kev = e.key;
			break;
	};
}

GameController::GameController(const uint32_t jid): jid(jid)
{
	gcsdl = SDL_GameControllerOpen(jid);
}

GameController::GameController(GameController && gameController): jid(gameController.jid)
{
	gcsdl = nullptr;
	std::swap(gcsdl, gameController.gcsdl);
}

GameController::~GameController()
{
	if (gcsdl)
		SDL_GameControllerClose(gcsdl);
}

void GameController::processEvent(const SDL_Event & e)
{
	switch (e.type) {
		case SDL_CONTROLLERAXISMOTION:
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			break;
		case SDL_CONTROLLERBUTTONUP:
			break;
	};
}

Joystick::Joystick(const uint32_t jid): jid(jid)
{
	joysdl = SDL_JoystickOpen(jid);
}

Joystick::Joystick(Joystick && joystick): jid(joystick.jid)
{
	joysdl = nullptr;
	std::swap(joysdl, joystick.joysdl);
}

Joystick::~Joystick()
{
	if (joysdl)
		SDL_JoystickClose(joysdl);
}

void Joystick::processEvent(const SDL_Event & e)
{
	switch (e.type) {
		case SDL_JOYAXISMOTION:
			break;
		case SDL_JOYBALLMOTION:
			break;
		case SDL_JOYHATMOTION:
			break;
		case SDL_JOYBUTTONDOWN:
			break;
		case SDL_JOYBUTTONUP:
			break;
	};
}


