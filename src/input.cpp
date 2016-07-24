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

#include "input.hpp"

#include <stdio.h>
#include <iostream>

#include "game.hpp"
#include "player.hpp"
#include "scene.hpp"
#include "scenemenu.hpp"

const char * InputManager::controllerDBfilename = "gamecontrollerdb.txt";

InputManager::InputManager()
{
	SDL_JoystickEventState(SDL_ENABLE);
	loadGameControllerDB();
	m_game = nullptr;
}

InputManager::~InputManager()
{
	for (size_t i = 0; i < m_devices.size(); i++)
		delete m_devices[i];
}

unsigned int InputManager::initialize(Game * game)
{
	m_game = game;
	for (size_t i = 0; i < m_devices.size(); i++)
		delete m_devices[i];
	m_devices.clear();
	m_devices.push_back(new KeyboardInput(*this));
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i))
			m_devices.push_back(new GameController(*this, i));
		else
			m_devices.push_back(new Joystick(*this, i));
	}
	for (auto i = m_devices.begin(); i != m_devices.end(); i++)
		(*i)->initialize();
	return m_devices.size();
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
	for (unsigned int i = 0; i < m_devices.size(); i++)
		m_devices[i]->receiveEvent(e);
}

void InputDevice::receiveEvent(const SDL_Event & e)
{
	inputstate_t eventstate = processEvent(e);
	if ((eventstate.d != INPUT_D_UNDEFINED ||
	        eventstate.a != INPUT_B_UNDEFINED ||
	        eventstate.b != INPUT_B_UNDEFINED ||
	        eventstate.c != INPUT_B_UNDEFINED ||
	        eventstate.x != INPUT_B_UNDEFINED ||
	        eventstate.y != INPUT_B_UNDEFINED ||
	        eventstate.z != INPUT_B_UNDEFINED ||
	        eventstate.start != INPUT_B_UNDEFINED ||
	        eventstate.back != INPUT_B_UNDEFINED) &&
		eventstate != m_previousChange) {
		m_previousChange = eventstate;
		m_manager.registerInput(this, eventstate);
		
		if (eventstate.d != INPUT_D_UNDEFINED)
			m_currentState.d = eventstate.d;
		
		if (eventstate.a != INPUT_B_UNDEFINED)
			m_currentState.a = eventstate.a;
		
		if (eventstate.b != INPUT_B_UNDEFINED)
			m_currentState.b = eventstate.b;
		
		if (eventstate.c != INPUT_B_UNDEFINED)
			m_currentState.c = eventstate.c;
		
		if (eventstate.x != INPUT_B_UNDEFINED)
			m_currentState.x = eventstate.x;
		
		if (eventstate.y != INPUT_B_UNDEFINED)
			m_currentState.y = eventstate.y;
		
		if (eventstate.z != INPUT_B_UNDEFINED)
			m_currentState.z = eventstate.z;
		
		if (eventstate.start != INPUT_B_UNDEFINED)
			m_currentState.start = eventstate.start;
		
		if (eventstate.back != INPUT_B_UNDEFINED)
			m_currentState.back = eventstate.back;
	}
}

InputDevice & InputManager::device(size_t n)
{
	return *m_devices[n];
}

void InputManager::assignDeviceToPlayer(InputDevice * device, Player * player)
{
	if (device)
		device->assignToPlayer(player);
}

void InputManager::registerInput(InputDevice * device, inputstate_t state)
{
	if (device && m_game) {
		Scene * scene = m_game->currentScene();
		SceneMenu * smenu;
		if (scene && (smenu = dynamic_cast<SceneMenu *>(scene))) {
			smenu->receiveInput(device, state);
		}
	}
}

const size_t InputManager::deviceNumber() const
{
	return m_devices.size();
}

const inputstate_t InputDevice::getState() const
{
	return m_currentState;
}

InputDevice::InputDevice(InputManager & manager): m_player(nullptr), m_manager(manager)
{
}

bool InputDevice::hasPlayerAssigned() const
{
	return (m_player != nullptr);
}

Player * InputDevice::getAssignedPlayer()
{
	return m_player;
}

inputstate_t InputDevice::getState()
{
	return m_currentState;
}

void InputDevice::initialize()
{
	updateGlobalState();
}

void InputDevice::assignToPlayer(Player * assignedPlayer)
{
	m_player = assignedPlayer;
}

KeyboardInput::KeyboardInput(InputManager & manager): InputDevice(manager)
{
}

inputstate_t KeyboardInput::processEvent(const SDL_Event & e)
{
	inputstate_t state;
	switch (e.type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		updateGlobalState();
		break;
	};
	return state;
}

const inputbutton KeyboardInput::evaluateKey(SDL_Scancode key)
{
	const uint8_t * keystate = SDL_GetKeyboardState(NULL);
	if (keystate[key])
		return INPUT_B_PRESSED;
	else
		return INPUT_B_RELEASED;
}

void KeyboardInput::updateGlobalState()
{
	m_currentState.a = evaluateKey(scancodeA);
	m_currentState.b = evaluateKey(scancodeB);
	m_currentState.c = evaluateKey(scancodeC);
	m_currentState.x = evaluateKey(scancodeX);
	m_currentState.y = evaluateKey(scancodeY);
	m_currentState.z = evaluateKey(scancodeZ);
	m_currentState.start = evaluateKey(scancodeStart);
	m_currentState.back = evaluateKey(scancodeBack);
	const uint8_t * keystate = SDL_GetKeyboardState(NULL);
	if (keystate[scancodeUp]) {
		if (keystate[scancodeRight]) {
			m_currentState.d = INPUT_D_NE;
		}
		else
			if (keystate[scancodeLeft]) {
				m_currentState.d = INPUT_D_NW;
			}
			else {
				m_currentState.d = INPUT_D_N;
			}
	}
	else
		if (keystate[scancodeDown]) {
			if (keystate[scancodeRight]) {
				m_currentState.d = INPUT_D_SE;
			}
			else
				if (keystate[scancodeLeft]) {
					m_currentState.d = INPUT_D_SW;
				}
				else {
					m_currentState.d = INPUT_D_S;
				}
		}
		else {
			if (keystate[scancodeRight]) {
				m_currentState.d = INPUT_D_E;
			}
			else
				if (keystate[scancodeLeft]) {
					m_currentState.d = INPUT_D_W;
				}
				else {
					m_currentState.d = INPUT_D_NEUTRAL;
				}
		}
}

GameController::GameController(InputManager & manager, const uint32_t jid): InputDevice(manager), m_jid(jid)
{
	m_gcsdl = SDL_GameControllerOpen(jid);
}

GameController::GameController(GameController && gameController): InputDevice(gameController.m_manager), m_jid(gameController.m_jid)
{
	m_gcsdl = nullptr;
	std::swap(m_gcsdl, gameController.m_gcsdl);
}

GameController::~GameController()
{
	if (m_gcsdl)
		SDL_GameControllerClose(m_gcsdl);
}

inputstate_t GameController::processEvent(const SDL_Event & e)
{
	inputstate_t state;
	switch (e.type) {
	case SDL_CONTROLLERAXISMOTION: {
		const SDL_ControllerAxisEvent & aevent = e.caxis;
		if (aevent.axis == SDL_CONTROLLER_AXIS_LEFTX || aevent.axis == SDL_CONTROLLER_AXIS_LEFTY) {
			inputdir dir = getDirection();
			state.d = dir;
		}
		else
			if (aevent.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
				state.z = getButtonValueForAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
			}
	}
	break;
	case SDL_CONTROLLERBUTTONDOWN: {
		const SDL_ControllerButtonEvent & bdevent = e.cbutton;
		if (bdevent.which == m_jid) {
			switch (bdevent.button) {
			case SDL_CONTROLLER_BUTTON_A:
				state.a = INPUT_B_PRESSED;
				break;
			case SDL_CONTROLLER_BUTTON_B:
				state.b = INPUT_B_PRESSED;
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				state.c = INPUT_B_PRESSED;
				break;
			case SDL_CONTROLLER_BUTTON_X:
				state.x = INPUT_B_PRESSED;
				break;
			case SDL_CONTROLLER_BUTTON_Y:
				state.y = INPUT_B_PRESSED;
				break;
			case SDL_CONTROLLER_BUTTON_START:
				state.start = INPUT_B_PRESSED;
				break;
			case SDL_CONTROLLER_BUTTON_BACK:
				state.back = INPUT_B_PRESSED;
				break;
			}
		}
	}
	break;
	case SDL_CONTROLLERBUTTONUP: {
		const SDL_ControllerButtonEvent & buevent = e.cbutton;
		if (buevent.which == m_jid) {
			switch (buevent.button) {
			case SDL_CONTROLLER_BUTTON_A:
				state.a = INPUT_B_RELEASED;
				break;
			case SDL_CONTROLLER_BUTTON_B:
				state.b = INPUT_B_RELEASED;
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				state.c = INPUT_B_RELEASED;
				break;
			case SDL_CONTROLLER_BUTTON_X:
				state.x = INPUT_B_RELEASED;
				break;
			case SDL_CONTROLLER_BUTTON_Y:
				state.y = INPUT_B_RELEASED;
				break;
			case SDL_CONTROLLER_BUTTON_START:
				state.start = INPUT_B_RELEASED;
				break;
			case SDL_CONTROLLER_BUTTON_BACK:
				state.back = INPUT_B_RELEASED;
				break;
			}
		}
	}
	break;
	};
	return state;
}

void GameController::updateGlobalState()
{
	SDL_GameControllerUpdate();
	m_currentState.a = getButtonValue(SDL_CONTROLLER_BUTTON_A);
	m_currentState.b = getButtonValue(SDL_CONTROLLER_BUTTON_B);
	m_currentState.c = getButtonValueForAxis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	m_currentState.x = getButtonValue(SDL_CONTROLLER_BUTTON_X);
	m_currentState.y = getButtonValue(SDL_CONTROLLER_BUTTON_Y);
	m_currentState.z = getButtonValue(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	m_currentState.start = getButtonValue(SDL_CONTROLLER_BUTTON_START);
	m_currentState.back = getButtonValue(SDL_CONTROLLER_BUTTON_BACK);
	m_currentState.d = getDirection();
}

inputbutton GameController::getButtonValue(SDL_GameControllerButton button)
{
	switch (SDL_GameControllerGetButton(m_gcsdl, button)) {
	case 0:
		return INPUT_B_RELEASED;
	case 1:
		return INPUT_B_PRESSED;
	};
	return INPUT_B_UNDEFINED;
}

inputbutton GameController::getButtonValueForAxis(SDL_GameControllerAxis axis)
{
	if (SDL_GameControllerGetAxis(m_gcsdl, axis) >= threshold)
		return INPUT_B_PRESSED;
	return INPUT_B_RELEASED;
}

inputdir GameController::getDirection(Sint16 hor, Sint16 vert)
{
	if (hor <= - threshold) {
		// Going to the left
		if (vert <= - threshold) {
			// Going up
			return INPUT_D_NW;
		}
		else
			if (vert >= threshold) {
				// Going down
				return INPUT_D_SW;
			}
		return INPUT_D_W;
	}
	else
		if (hor >= threshold) {
			if (vert <= - threshold) {
				// Going up
				return INPUT_D_NE;
			}
			else
				if (vert >= threshold) {
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
	else
		if (vert >= threshold) {
			// Going down
			return INPUT_D_S;
		}
	return INPUT_D_NEUTRAL;
}

inputdir GameController::getDirection()
{
	Sint16 vert = SDL_GameControllerGetAxis(m_gcsdl, SDL_CONTROLLER_AXIS_LEFTY);
	Sint16 hor = SDL_GameControllerGetAxis(m_gcsdl, SDL_CONTROLLER_AXIS_LEFTX);
	return getDirection(hor, vert);
}

Joystick::Joystick(InputManager & manager, const uint32_t jid): InputDevice(manager), m_jid(jid)
{
	m_joysdl = SDL_JoystickOpen(jid);
}

Joystick::Joystick(Joystick && joystick): InputDevice(joystick.m_manager), m_jid(joystick.m_jid)
{
	m_joysdl = nullptr;
	std::swap(m_joysdl, joystick.m_joysdl);
}

Joystick::~Joystick()
{
	if (m_joysdl)
		SDL_JoystickClose(m_joysdl);
}

inputstate_t Joystick::processEvent(const SDL_Event & e)
{
	inputstate_t state;
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
	return state;
}

void Joystick::updateGlobalState()
{

}


