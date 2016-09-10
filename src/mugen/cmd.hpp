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

#ifndef CMD_H
#define CMD_H
#include "../input.hpp"

#include <vector>
#include <string>
#include <regex>


namespace Nugem {
namespace Mugen {

class CharacterCommands {
public:
	CharacterCommands() {};
	void readFile(const std::string & filepath);
protected:
	
	enum CommandButton { a, b, c, x, y, z };

	enum CommandDirection { B, DB, D, DF, F, UF, U, UB };

	struct CommandButtonModif {
		bool heldDown = false;
		bool released = false;
		unsigned int chargeTicks = 0; // the number of game ticks the key must be held down for
		CommandButton button;
	};

	struct CommandInput {
		bool exclusive = false; // '>' modifier
	};

	struct CommandInputDirection: public CommandInput {
		bool heldDown = false;
		bool released = false;
		unsigned int chargeTicks = 0; // the number of game ticks the key must be held down for
		bool wideDirection = false; // detect the direction as 4-way
		CommandDirection direction;
		CommandInputDirection(CommandDirection _direction): direction(_direction) {};
	};

	struct CommandInputButtons: public CommandInput {
		std::vector<CommandButtonModif> symbols; // simultaneous button events
	};

	struct CommandDefinition {
		std::string name;
		std::vector<std::unique_ptr<CommandInput>> inputs;
		int16_t time;
		int16_t buffertime;
	};

	class StateEntry {};

	std::vector<std::unique_ptr<CommandInput>> readInputDefinition(const std::string & entryString);
private:
	std::vector<std::unique_ptr<CommandDefinition>> m_commands;
	std::vector<StateEntry> m_stateEntries;
	static const std::regex sectionCommand;
	static const std::regex commandName;
	static const std::regex commandCmd;
	static const std::regex commandTime;
	static const std::regex commandBufferTime;
	static const std::regex sectionStateEntry;
};

}
}

#endif // CMD_H
