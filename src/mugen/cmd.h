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

#ifndef CMD_H
#define CMD_H
#include "../input.h"

#include <vector>
#include <string>
#include <regex>

namespace mugen {

class CommandDefinition {
protected:
	std::string name;
	std::vector<inputstate_t> commands;
	int16_t time;
	int16_t buffertime;
};

class StateEntry {};

class CharacterCommands {
public:
	CharacterCommands() {};
	CharacterCommands(const std::string & cmdfile);
	CharacterCommands(CharacterCommands && characterCommands);
private:
	std::vector<CommandDefinition> commandDefinitions;
	std::vector<StateEntry> stateEntries;
	static const std::regex sectionCommand;
	static const std::regex commandName;
	static const std::regex commandCmd;
	static const std::regex commandTime;
	static const std::regex commandBufferTime;
	static const std::regex sectionStateEntry;
};

}

#endif // CMD_H