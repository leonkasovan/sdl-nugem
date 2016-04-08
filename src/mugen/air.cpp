#include "air.h"



mugen::AnimationData::AnimationData()
{
}

mugen::AnimationData::AnimationData(mugen::AnimationData && animationData): std::map<size_t, animation_t>(animationData)
{
}

const std::regex mugen::AnimationData::regexSection("Begin Action ([0-9]+)");
const std::regex mugen::AnimationData::regexClsnInit("[ \t\r\n]*(Clsn([0-9]+)(?:Default)?):[ \t]*([0-9]+)[ \t\r\n]*");
const std::regex mugen::AnimationData::regexClsn("[ \t\r\n]*Clsn2\\[([0-9]+)\\][ \t]*=[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+)[ \t\r\n]*");
const std::regex mugen::AnimationData::regexStep("[ \t\r\n]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+),[ \t]*(-?[0-9]+)((?:[ \t]*,[ \t]*[A-Za-z0-9]*)*)[ \t\r\n]*");

mugen::AnimationData::AnimationData(const std::string & filepath)
{
	readFile(filepath);
}

mugen::AnimationData & mugen::AnimationData::readFile(const std::string & filepath)
{
	mugen::MugenTextFile air(filepath);
	std::string line;
	mugen::animation_t currentAnimation;
	int actionNumber = -1;
	for (line = air.nextLine(); !line.empty(); line = air.nextLine()) {
		std::smatch sm;
		// new action start (i.e. a section describing an animation)
		if (air.newSection()) {
			// first, if the current action is named, we add it
			if (actionNumber >= 0) {
				(*this)[actionNumber] = currentAnimation;
			}
			// find the number of the new action
			if (!std::regex_match(air.section(), sm, regexSection)) {
				// if a number can't be found then we just skip this line and section
				actionNumber = -1;
				continue;
			}
			// then start a new action
			actionNumber = std::stoi(sm[1]);
			currentAnimation = mugen::animation_t();
			continue;
		}
		// if we're not actually in a numbered action then we skip this key-value pair
		if (actionNumber < 0)
			continue;
		// we want a line with 5 numbers separated by commas
		// Note: Putting -1,0 for the sprite means it does not draw anything
		// elem is of the form "x,y" with x image group and y image number in group
		if (std::regex_match(line, sm, regexStep)) {
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
	if (actionNumber >= 0)
		(*this)[actionNumber] = currentAnimation;
}

mugen::AnimationData & mugen::AnimationData::operator=(mugen::AnimationData && animationData)
{
	std::map<size_t, animation_t>::operator=(animationData);
	return *this;
}

mugen::AnimationData & mugen::AnimationData::operator=(const mugen::AnimationData & animationData)
{
	std::map<size_t, animation_t>::operator=(animationData);
	return *this;
}

