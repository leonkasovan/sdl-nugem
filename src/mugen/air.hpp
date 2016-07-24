#ifndef AIR_H
#define AIR_H

#include "mugenutils.hpp"

namespace mugen {

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
	size_t framenumber;
};

struct animboxlist_t {
	size_t framenumber;
	
};

struct animation_t {
	std::vector<animbox_t> boxes;
	std::vector<animstep_t> steps;
	size_t loopstart;
	animation_t() {
			loopstart = 0;
	}
};

class AnimationData: public std::map<size_t, animation_t> {
public:
	AnimationData();
	AnimationData(const std::string & filepath);
	AnimationData(AnimationData && animationData);
	AnimationData& operator=(AnimationData && animationData);
	AnimationData& operator=(const AnimationData & animationData);
	AnimationData& readFile(const std::string & filepath);
	static const std::regex regexSection;
	static const std::regex regexClsnInit;
	static const std::regex regexClsn;
	static const std::regex regexStep;
};

}

#endif // AIR_H