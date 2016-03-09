#ifndef MUGENUTILS_H
#define MUGENUTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

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
	const defkey& at ( const std::string& k) const {
		return contents.at(k);
	}
	defkey& at ( const std::string& k) {
		return contents.at(k);
	}
};

struct animstep_t {
	// values separated by a comma in the file
	size_t group;
	size_t image;
	unsigned int x;
	unsigned int y;
	unsigned int ticks; // duration. Unit: 1/60 of a second
	bool hinvert; // horizontal inversion
	bool vinvert; // vertical inversion
};

struct animbox_t {
	enum { COLLISION, ATTACK } type;
	unsigned int coordinates[4];
};

struct animation_t {
	std::vector<animbox_t> boxes;
	std::vector<animstep_t> steps;
	size_t loopstart;
	animation_t() {
			loopstart = 0;
	}
};

typedef std::unordered_map<std::string, defsection> defcontents;
typedef std::map<size_t, animation_t> animationdict;

// std::getline adjusted for the ; comment character
std::istream& _getline(std::istream& __is, std::__cxx11::string& __str);
defcontents loadDef(const char* filepath);
animationdict loadAir(const char* filepath);

}

#endif // MUGENUTILS_H