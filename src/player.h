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

#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include "input.h"

#include <deque>

class Player {
public:
	Player();
	virtual ~Player();
	void setCharacter(Character * c);
	Character * getCharacter();
	void setInputDevice(InputDevice * iD);
	InputDevice * getInputDevice();
	void receiveInput(inputstate_t & inputState);
protected:
	Character * character;
	InputDevice * inputDevice;
	std::deque<inputstate_t> inputs;
};

#endif // PLAYER_H