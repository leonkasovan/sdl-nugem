#include "mugenutils.h"

#include <fstream>
#include <sstream>
#include <iostream>

mugen::defcontents mugen::loadDef(const char * filepath)
{
	mugen::defcontents s;
	std::ifstream defs(filepath);
	std::string line;
	std::string currentSection;
	while (std::getline(defs, line)) {
		// Cut the line at the comments: find the ; character
		bool ignored = false;
		for (unsigned int index = 0; index < line.size(); index++) {
			if (ignored) {
				if (line[index] == '\"')
					ignored = false;
				continue;
			}
			// else
			switch (line[index]) {
			case '"':
				ignored = true;
				break;
			case ';': // do not process comments
				line = line.substr(0, index);
				index = line.size();
				break;
			}
		}
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
