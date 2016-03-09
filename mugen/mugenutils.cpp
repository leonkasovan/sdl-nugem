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
	std::regex sectionregex("[ \t\r\n]*\\[([0-9A-Za-z]+)\\][ \t\r\n]*");
	std::regex valueregex("[ \t\r\n]*([^ \t=]+)[ \t]*=[ \t]*([^ \t\r\n]+)[ \t\r\n]*");
	std::regex stringregex("[ \t\r\n]*([^ \t=]+)[ \t]*=[ \t]*\"([^\"]*)\"[ \t\r\n]*");
	while (mugen::_getline(defs, line)) {
		std::smatch sm;
		std::string identifier;
		// Check if it's a section
		if (std::regex_match(line, sm, sectionregex)) {
			currentSection = sm[1];
			continue;
		}
		mugen::defkey newkey;
		// Otherwise check if it's a value
		if (std::regex_match(line, sm, stringregex)) {
			newkey.keytype = defkey::STRING;
			identifier = sm[1];
			newkey.value = sm[2];
		}
		else if (std::regex_match(line, sm, valueregex)) {
			newkey.keytype = defkey::SIMPLE;
			identifier = sm[1];
			newkey.value = sm[2];
		}
		else {
			continue;
		}
		std::transform(identifier.begin(), identifier.end(), identifier.begin(), ::tolower);
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
	std::regex sectionregex("[ \t\r\n]*\\[Begin Action ([0-9]+)\\][ \t\r\n]*");
	std::regex clsninitregex("[ \t\r\n]*(Clsn2(?:Default)?):[ \t]*([0-9]+)[ \t\r\n]*");
	std::regex clsnregex("[ \t\r\n]*Clsn2\\[([0-9]+)\\][ \t]*=[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+)[ \t\r\n]*");
	std::regex stepregex("[ \t\r\n]*([0-9]+),[ \t]*([0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*([0-9]+)((?:[ \t]*,[ \t]*[A-Za-z0-9]*)*),?[ \r\n\t]*");
	while (mugen::_getline(air, line)) {
		std::smatch sm;
		// new action start (i.e. a section describing an animation)
		if (std::regex_match(line, sm, sectionregex)) {
			// first, if the current action is named, we add it
			if (currentSection >= 0) {
				s[currentSection] = currentAnimation;
			}
			// then start a new action
			currentSection = std::stoi(sm[1]);
			currentAnimation = mugen::animation_t();
			continue;
		}
		// if it's not a new action, but a line with 5 numbers separated by commas
		// Note: Putting -1,0 for the sprite means it does not draw anything
		// elem is of the form "x,y" with x image group and y image number in group
		if (std::regex_match(line, sm, stepregex)) {
			// start at index 1 of sm because index 0 is the whole string
			mugen::animstep_t step;
			step.group = std::stoi(sm[1]);
			step.image = std::stoi(sm[2]);
			step.x = std::stoi(sm[3]);
			step.y = std::stoi(sm[4]);
			step.ticks = std::stoi(sm[5]);
			if (sm.size() > 6) { // horizontal and vertical inversion
				const std::string arg(sm[6]);
				size_t hinv = arg.find('H');
				step.hinvert = (hinv != std::string::npos);
				size_t vinv = arg.find('V');
				step.vinvert = (vinv != std::string::npos);
			}
			else {
				step.hinvert = false;
				step.vinvert = false;
			}
			if (sm.size() > 7) {}
			currentAnimation.steps.push_back(step);
		}
		else if (line.find("LoopStart") != std::string::npos) {
			currentAnimation.loopstart = currentAnimation.steps.size();
		}
			
	}
	air.close();
	return s;
}

