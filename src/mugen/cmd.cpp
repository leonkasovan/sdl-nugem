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

#include "cmd.h"
#include "mugenutils.h"
#include <fstream>
#include <string>
#include <deque>

mugen::CharacterCommands::CharacterCommands(const std::string & cmdfile)
{
	std::ifstream file = std::ifstream(cmdfile);
	std::string line;
	while (mugen::_getline(file, line)) {
	}
	file.close();
}

const std::regex mugen::CharacterCommands::sectionCommand("[  \t\r\n]*\\[Command\\][ \t\r\n]*");
const std::regex mugen::CharacterCommands::commandName("[ \t\r\n]*name[ \t]*=[ \t]*([^\r\n]+?)|(?:\"([^\"]|(?:\\\\\"))*\")[ \t\r\n]*");
const std::regex mugen::CharacterCommands::commandCmd("[ \t\r\n]*command[ \t]*=[ \t]*((?:[a-cx-zBDFU\\~\\/\\$\\+\\>0-9]+,?[ \t]*?)*)[ \t\r\n]*");
const std::regex mugen::CharacterCommands::commandTime("[ \t\r\n]*time[ \t]*=[ \t]*(-?[0-9]*)[ \t\r\n]*");
const std::regex mugen::CharacterCommands::commandBufferTime("[ \t\r\n]*buffer\\.time[ \t]*=[ \t]*(-?[0-9]*)[ \t\r\n]*");
const std::regex mugen::CharacterCommands::sectionStateEntry("");

mugen::CharacterCommands::CharacterCommands(mugen::CharacterCommands && characterCommands)
{

}
