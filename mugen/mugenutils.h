#ifndef MUGENUTILS_H
#define MUGENUTILS_H

#include <string>
#include <unordered_map>

namespace mugen {

struct defkey {
	enum keytype_t { SIMPLE, STRING } keytype;
	std::string value;
	operator const char * () const {
		return value.c_str();
	}
};

struct defsection {
	std::string name;
	std::unordered_map<std::string, defkey> contents;
	defkey& operator[] (const std::string& k) {
		return contents[k];
	}
	defkey& operator[] (std::string&& k) {
		return contents[k];
	}
};

typedef std::unordered_map<std::string, defsection> defcontents;

defcontents loadDef(const char* filepath);

}

#endif // MUGENUTILS_H