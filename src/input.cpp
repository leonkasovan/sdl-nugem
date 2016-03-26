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
	for (auto i = devices.begin(); i != devices.end(); i++)
		(*i)->initialize();
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

void InputDevice::initialize()
{
	updateState();
}

void InputDevice::assignToPlayer(Player * assignedPlayer)
{
	player = assignedPlayer;
}

KeyboardInput::KeyboardInput()
{
	keyA = SDL_GetKeyFromScancode(scancodeA);
	keyB = SDL_GetKeyFromScancode(scancodeB);
	keyC = SDL_GetKeyFromScancode(scancodeC);
	keyX = SDL_GetKeyFromScancode(scancodeX);
	keyY = SDL_GetKeyFromScancode(scancodeY);
	keyZ = SDL_GetKeyFromScancode(scancodeZ);
	keyStart = SDL_GetKeyFromScancode(scancodeStart);
	keyUp = SDL_GetKeyFromScancode(scancodeUp);
	keyDown = SDL_GetKeyFromScancode(scancodeDown);
	keyLeft = SDL_GetKeyFromScancode(scancodeLeft);
	keyRight = SDL_GetKeyFromScancode(scancodeRight);
}

void KeyboardInput::processEvent(const SDL_Event & e)
{
	switch (e.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			updateState();
			break;
	};
}

const inputbutton KeyboardInput::evaluateKey(SDL_Keycode key)
{
	const uint8_t * keystate = SDL_GetKeyboardState(NULL);
	if (keystate[keyA])
		return INPUT_B_PRESSED;
	else
		return INPUT_B_RELEASED;
}

void KeyboardInput::updateState()
{
	currentState.a = evaluateKey(keyA);
	currentState.b = evaluateKey(keyB);
	currentState.c = evaluateKey(keyC);
	currentState.x = evaluateKey(keyX);
	currentState.y = evaluateKey(keyY);
	currentState.z = evaluateKey(keyZ);
	currentState.start = evaluateKey(keyStart);
	const uint8_t * keystate = SDL_GetKeyboardState(NULL);
	if (keystate[keyUp]) {
		if (keystate[keyRight]) {
			currentState.d = INPUT_D_NE;
		}
		else if (keystate[keyLeft]) {
			currentState.d = INPUT_D_NW;
		}
		else {
			currentState.d = INPUT_D_N;
		}}
	else if (keystate[keyDown]) {
		if (keystate[keyRight]) {
			currentState.d = INPUT_D_SE;
		}
		else if (keystate[keyLeft]) {
			currentState.d = INPUT_D_SW;
		}
		else {
			currentState.d = INPUT_D_S;
		}}
	else {
		if (keystate[keyRight]) {
			currentState.d = INPUT_D_E;
		}
		else if (keystate[keyLeft]) {
			currentState.d = INPUT_D_W;
		}
		else {
			currentState.d = INPUT_D_NEUTRAL;
		}
	}
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
			getDirection();
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			break;
		case SDL_CONTROLLERBUTTONUP:
			break;
	};
	if (player)
		player->receiveInput(currentState);
}

void GameController::updateState()
{
	SDL_GameControllerUpdate();
	currentState.a = getButtonValue(SDL_CONTROLLER_BUTTON_A);
	currentState.b = getButtonValue(SDL_CONTROLLER_BUTTON_B);
	currentState.c = getButtonValueForAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	currentState.x = getButtonValue(SDL_CONTROLLER_BUTTON_X);
	currentState.y = getButtonValue(SDL_CONTROLLER_BUTTON_Y);
	currentState.z = getButtonValue(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	currentState.start = getButtonValue(SDL_CONTROLLER_BUTTON_START);
	currentState.d = getDirection();
}

inputbutton GameController::getButtonValue(SDL_GameControllerButton button)
{
	switch (SDL_GameControllerGetButton(gcsdl, button)) {
		case 0:
			return INPUT_B_RELEASED;
		case 1:
			return INPUT_B_PRESSED;
	};
	return INPUT_B_UNDEFINED;
}

inputbutton GameController::getButtonValueForAxis(SDL_GameControllerAxis axis)
{
	if (SDL_GameControllerGetAxis(gcsdl, axis) >= threshold)
		return INPUT_B_PRESSED;
	return INPUT_B_RELEASED;
}

inputdir GameController::getDirection()
{
	Sint16 vert, hor;
	hor = SDL_GameControllerGetAxis(gcsdl, SDL_CONTROLLER_AXIS_LEFTX);
	vert = SDL_GameControllerGetAxis(gcsdl, SDL_CONTROLLER_AXIS_LEFTY);
	if (hor <= - threshold) {
		// Going to the left
		if (vert <= - threshold) {
			// Going up
			return INPUT_D_NW;
		}
		else if (vert >= threshold) {
			// Going down
			return INPUT_D_SW;
		}
		return INPUT_D_W;
	}
	else if (hor >= threshold) {
		if (vert <= - threshold) {
			// Going up
			return INPUT_D_NE;
		}
		else if (vert >= threshold) {
			// Going down
			return INPUT_D_SE;
		}
		// Going to the right
		return INPUT_D_E;
	}
	if (vert <= - threshold) {
		// Going up
		return INPUT_D_N;
	}
	else if (vert >= threshold) {
		// Going down
		return INPUT_D_S;
	}
	return INPUT_D_NEUTRAL;
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

void Joystick::updateState()
{

}


