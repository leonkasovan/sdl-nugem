#include "mugenutils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>

std::istream & mugen::_getline(std::istream & __is, std::__cxx11::string & __str)
{
	std::istream & _is = std::getline(__is, __str);
	// Cut the line at the comments: find the ; character
	bool ignored = false;
	for (unsigned int index = 0; index < __str.size(); index++) {
		if (ignored) {
			if (__str[index] == '\"')
				ignored = false;
			continue;
		}
		// else
		switch (__str[index]) {
		case '"':
			ignored = true;
			break;
		case ';': // do not process comments
			__str = __str.substr(0, index);
			index = __str.size();
			break;
		}
	}
	return _is;
}

mugen::defcontents mugen::loadDef(const char * filepath)
{
	mugen::defcontents s;
	std::ifstream defs(filepath);
	std::string line;
	std::string currentSection;
	while (mugen::_getline(defs, line)) {
		// Processing the line
		std::stringstream linestream(line);
		std::string identifier, separator, value;
		linestream >> identifier;
		// Check if it's a section
		if (identifier[0] == '[' && identifier[identifier.size() - 1] == ']') {
			currentSection = identifier.substr(1, identifier.size() - 2);
		}
		// Otherwise check if it's a value
		linestream >> separator;
		if (separator != "=")
			continue;
		std::getline(linestream, value);
		// trim the value string
		size_t start = value.find_first_not_of(" \t\r");
		size_t end = value.find_last_not_of(" \t\r");
		value = value.substr(start, end + 1 - start);
		mugen::defkey newkey;
		if (value[0] == '"' && value[value.size() - 1] == '"') {
			newkey.keytype = defkey::STRING;
			newkey.value = value.substr(1, value.size() - 2);
		}
		else {
			newkey.keytype = defkey::SIMPLE;
			newkey.value = value;
		}
		s[currentSection][identifier] = newkey;
	}
	defs.close();
	return s;
}

mugen::animationdict mugen::loadAir(const char * filepath)
{
	mugen::animationdict s;
	std::ifstream air(filepath);
	std::string line;
	int currentSection = -1;
	mugen::animation_t currentAnimation;
	std::regex stepregex("[ \t\r\n]*([0-9]+),[ \t]*([0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*([0-9]+)[ \t,A-Z]*[ \r\n\t]*");
	while (mugen::_getline(air, line)) {
		std::stringstream linestream(line);
		std::string elem;
		linestream >> elem;
		// new action start (i.e. a section describing an animation)
		if (elem == "[Begin") {
			linestream >> elem;
			if (elem != "Action")
				continue;
			linestream >> elem;
			if (elem[elem.size() - 1] != ']')
				continue;
			// first, if the current action is named, we add it
			if (currentSection >= 0) {
				s[currentSection] = currentAnimation;
			}
			// then start a new action
			currentSection = std::stoi(elem.substr(0, elem.size() - 1));
			currentAnimation.steps.clear();
			currentAnimation.boxes.clear();
			continue;
		}
		std::smatch sm;
		// if it's not a new action, but a line with 5 numbers separated by commas
		// Note: Putting -1,0 for the sprite means it does not draw anything
		// elem is of the form "x,y" with x image group and y image number in group
		if (std::regex_match(line, sm, stepregex)/* && sm.size() >= 6*/) {
			// start at index 1 of sm because index 0 is the whole string
			mugen::animstep_t step;
			step.group = std::stoi(sm[1]);
			step.image = std::stoi(sm[2]);
			step.x = std::stoi(sm[3]);
			step.y = std::stoi(sm[4]);
			step.ticks = std::stoi(sm[5]);
			currentAnimation.steps.push_back(step);
		}
	}
	air.close();
	return s;
}

