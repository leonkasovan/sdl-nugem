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

#ifndef INPUT_H
#define INPUT_H

#include <vector>

#include <SDL.h>

#define INPUTSTATE_NBUTTONS 7
#define INPUTSTATE_BUTTON_A 0
#define INPUTSTATE_BUTTON_B 1
#define INPUTSTATE_BUTTON_C 2
#define INPUTSTATE_BUTTON_X 3
#define INPUTSTATE_BUTTON_Y 4
#define INPUTSTATE_BUTTON_Z 5
#define INPUTSTATE_BUTTON_S 6
// A single input state
// This input layout is lifted straight from Mugen
struct inputstate_t {
	// 7 buttons: A, B, C, X, Y, Z, Start (in order)
	uint8_t b[INPUTSTATE_NBUTTONS];
	// 9 directions:
	// 7 8 9
	// 4 5 6
	// 1 2 3
	uint8_t d;
	inputstate_t() {
		d = 5; // neutral
		for (int i = 0; i < INPUTSTATE_NBUTTONS; i++)
			b[i] = 0;
	}
};

class Player;

class InputDevice {
public:
	InputDevice();
	virtual ~InputDevice() {};
	virtual void processEvent(const SDL_Event & e) = 0;
	void setState(inputstate_t state);
	inputstate_t getState();
	const inputstate_t getState() const;
	void assignToPlayer(Player * assignedPlayer);
	bool hasPlayerAssigned() const;
protected:
	inputstate_t currentState;
	Player * player;
};

class KeyboardInput: public InputDevice {
public:
	virtual void processEvent(const SDL_Event & e);
	
};

class Joystick: public InputDevice {
public:
	Joystick(const uint32_t jid);
	Joystick(Joystick && joystick);
	virtual ~Joystick();
	virtual void processEvent(const SDL_Event & e);
private:
	const uint32_t jid;
	SDL_Joystick * joysdl;
};

class GameController: public InputDevice {
public:
	GameController(const uint32_t jid);
	GameController(GameController && gameController);
	virtual ~GameController();
	virtual void processEvent(const SDL_Event & e);
private:
	const uint32_t jid;
	SDL_GameController * gcsdl;
};

class InputManager {
public:
	InputManager();
	~InputManager();
	void processSDLEvent(const SDL_Event& e);
	const InputDevice & getDevice(size_t n) const;
	const size_t getDeviceNumber() const;
protected:
	std::vector<InputDevice *> devices;
	bool loadGameControllerDB();
	static const char * controllerDBfilename;
};

#endif // INPUT_H