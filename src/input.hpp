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

#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <memory>
#include <functional>

#include <SDL.h>

/*! \file input.h
 * Input methods definitions.
 */

namespace Nugem {

class Player;
class Game;

/**
* \brief Possible input states for a given button
*/
enum InputButtonState {
	INPUT_B_UNDEFINED = 0,
	INPUT_B_RELEASED = 1,
	INPUT_B_PRESSED = 2
};

/**
 * \brief Possible input values for the directional stick
 * 
 * There are 9 possible values, apart from "undefined", corresponding to the following map:
 * 
 * 7 8 9
 * 
 * 4 5 6
 * 
 * 1 2 3
 */
enum InputDirection {
	INPUT_D_UNDEFINED = 0,
	INPUT_D_SW = 1,
	INPUT_D_S = 2,
	INPUT_D_SE = 3,
	INPUT_D_W = 4,
	INPUT_D_NEUTRAL = 5,
	INPUT_D_E = 6,
	INPUT_D_NW = 7,
	INPUT_D_N = 8,
	INPUT_D_NE = 9
};

/** \brief A single input state.
 * This input layout is lifted straight from Mugen.
 */
struct InputState {
	InputButtonState a = INPUT_B_UNDEFINED;
	InputButtonState b = INPUT_B_UNDEFINED;
	InputButtonState c = INPUT_B_UNDEFINED;
	InputButtonState x = INPUT_B_UNDEFINED;
	InputButtonState y = INPUT_B_UNDEFINED;
	InputButtonState z = INPUT_B_UNDEFINED;
	InputButtonState start = INPUT_B_UNDEFINED;
	InputButtonState back = INPUT_B_UNDEFINED;
	InputDirection d = INPUT_D_UNDEFINED;
	bool operator != (const InputState & state) {
		return a != state.a ||
			b != state.b ||
			c != state.c ||
			x != state.x ||
			y != state.y ||
			z != state.z ||
			start != state.start ||
			back != state.back ||
			d != state.d;
	}
};
class InputManager;

class InputDevice {
public:
	InputDevice(InputManager & manager);
	virtual ~InputDevice() {};
	void receiveEvent(const SDL_Event & e );
	virtual void updateGlobalState() = 0;
	void initialize();
	InputState getState();
	const InputState getState() const;
	void assignToPlayer(Player * assignedPlayer);
	bool hasPlayerAssigned() const;
	Player * getAssignedPlayer();
protected:
	virtual InputState processEvent(const SDL_Event & e) = 0;
	InputState mCurrentState;
	Player *mPlayer;
	InputManager & m_manager;
private:
	   InputState m_previousChange;
};

class KeyboardInput: public InputDevice {
public:
	KeyboardInput(InputManager & manager);
	virtual InputState processEvent(const SDL_Event & e);
	virtual void updateGlobalState();
private:
	const InputButtonState evaluateKey(SDL_Scancode key);
	static const SDL_Scancode scancodeA = SDL_SCANCODE_A;
	static const SDL_Scancode scancodeB = SDL_SCANCODE_S;
	static const SDL_Scancode scancodeC = SDL_SCANCODE_D;
	static const SDL_Scancode scancodeX = SDL_SCANCODE_Q;
	static const SDL_Scancode scancodeY = SDL_SCANCODE_W;
	static const SDL_Scancode scancodeZ = SDL_SCANCODE_E;
	static const SDL_Scancode scancodeStart = SDL_SCANCODE_RETURN;
	static const SDL_Scancode scancodeUp = SDL_SCANCODE_UP;
	static const SDL_Scancode scancodeDown = SDL_SCANCODE_DOWN;
	static const SDL_Scancode scancodeLeft = SDL_SCANCODE_LEFT;
	static const SDL_Scancode scancodeRight = SDL_SCANCODE_RIGHT;
	static const SDL_Scancode scancodeBack = SDL_SCANCODE_ESCAPE;
};

class Joystick: public InputDevice {
public:
	Joystick(InputManager & manager, const uint32_t jid);
	Joystick(Joystick && joystick);
	virtual ~Joystick();
	virtual InputState processEvent(const SDL_Event & e);
	virtual void updateGlobalState();
private:
	const uint32_t m_jid;
	SDL_Joystick * m_joysdl;
};

class GameController: public InputDevice {
public:
	GameController(InputManager & manager, const uint32_t jid);
	GameController(GameController && gameController);
	virtual ~GameController();
	virtual InputState processEvent(const SDL_Event & e);
	virtual void updateGlobalState();
protected:
	   InputButtonState getButtonValue(SDL_GameControllerButton button);
	   InputButtonState getButtonValueForAxis(SDL_GameControllerAxis axis);
	   InputDirection getDirection(Sint16 hor, Sint16 vert);
	   InputDirection getDirection();
private:
	const uint32_t m_jid;
	SDL_GameController * m_gcsdl;
	static const Sint16 threshold = 32767 / 3;
};

class InputReceiver {
public:
	virtual ~InputReceiver() {};
	virtual void receiveInput(InputDevice *, InputState &) = 0;
};

class InputManager {
public:
	InputManager();
	~InputManager();
	unsigned int initialize(Game * game);
	void processSDLEvent(const SDL_Event& e);
	InputDevice& device(size_t n);
	const size_t deviceNumber() const;
	void addReceiver(InputReceiver *);
	void removeReceiver(InputReceiver *);
	void registerInput(InputDevice * device, InputState &state);
	void assignDeviceToPlayer(InputDevice * device, Player * player);
protected:
	std::vector<std::unique_ptr<InputDevice>> mDevices;
	std::vector<InputReceiver *> mReceivers;
	Game * mGame;
	bool loadGameControllerDB();
	static const char * controllerDBfilename;
};

}

#endif // INPUT_H
