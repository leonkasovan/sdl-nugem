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

#include "stage.hpp"

#include "mugenutils.hpp"

#include <regex>
#include <iostream>

namespace Nugem {
namespace Mugen {

Stage::Stage(const std::string &stageName): m_loadingName(stageName) {
// 	m_definition.reset(new DefinitionFile(std::string(folder) + "/" + stageName + ".def"));
// 	m_name = (*m_definition)["info"]["name"];
// 	m_displayName = (*m_definition)["info"]["displayname"];
// 	m_author = (*m_definition)["info"]["author"];
//
// 	if (!m_spriteLoader.isInitialized())
// 		m_spriteLoader.initialize(std::string(folder) + "/" + (*m_definition)["bgdef"]["spr"]);
}

void Stage::initialize() {
    MugenTextFile definitionFile(std::string(folder) + "/" + m_loadingName + ".def");
    MugenTextKeyValue kv;
    enum { DS_None, DS_Info, DS_Camera, DS_PlayerInfo, DS_Bound, DS_StageInfo, DS_Shadow, DS_Reflection, DS_Music, DS_BGDef } DefSection = DS_None;
	auto setString = [&](std::string &result) {
		result = kv.value();
	};
	auto setInt = [&](int &result) {
		result = std::stoi(kv.value());
	};
	auto setDouble = [&](double &result) {
		result = std::stod(kv.value());
	};
	bool newBgSection = false;
	std::string newBgSectionName = "";
	BgElement *currentBgSection = nullptr;
	std::unordered_map<decltype(DefSection),std::unordered_map<std::string, std::function<void(void)>>> kvEntryExecutor {
		{DS_Info, {
			{"name", [&]() { setString(m_name); }},
			{"displayname", [&]() { setString(m_displayName); }},
			{"versiondate", [&]() { setString(m_versionDate); }},
			{"mugenversion", [&]() { setString(m_mugenVersion); }},
			{"author", [&]() { setString(m_author); }}
		}},
		{DS_Camera, {
			{"startx", [&]() { setInt(m_start[0]); }},
			{"starty", [&]() { setInt(m_start[1]); }},
			{"boundleft", [&]() { setInt(m_boundleft); }},
			{"boundright", [&]() { setInt(m_boundright); }},
			{"boundhigh", [&]() { setInt(m_boundhigh); }},
			{"boundlow", [&]() { setInt(m_boundlow); }},
			{"tension", [&]() { setInt(m_tension); }},
			{"tensionhigh", [&]() { setInt(m_tensionhigh); }},
			{"tensionlow", [&]() { setInt(m_tensionlow); }},
			{"verticalfollow", [&]() { setDouble(m_verticalfollow); }},
			{"floortension", [&]() { setInt(m_floortension); }},
			{"overdrawhigh", [&]() { setInt(m_overdrawhigh); }},
			{"overdrawlow", [&]() { setInt(m_overdrawlow); }},
			{"cuthigh", [&]() { setInt(m_cuthigh); }},
			{"cutlow", [&]() { setInt(m_cutlow); }},
			{"startzoom", [&]() { setDouble(m_startzoom); }},
			{"zoomout", [&]() { setDouble(m_zoomout); }},
			{"zoomin", [&]() { setDouble(m_zoomin); }},
		}},
		{DS_PlayerInfo, {
			{"p1startx", [&]() { setInt(m_p1start[0]); }},
			{"p1starty", [&]() { setInt(m_p1start[1]); }},
			{"p2startx", [&]() { setInt(m_p2start[0]); }},
			{"p2starty", [&]() { setInt(m_p2start[1]); }},
			{"p1facing", [&]() { int val; setInt(val); m_p1facing = (val > 0); }},
			{"p2facing", [&]() { int val; setInt(val); m_p2facing = (val > 0); }},
			{"leftbound", [&]() { setInt(m_playerleftbound); }},
			{"rightbound", [&]() { setInt(m_playerrightbound); }}
		}},
		{DS_Bound, {
			{"screenleft", [&]() { setInt(m_screenleft); }},
			{"screeenright", [&]() { setInt(m_screenright); }}
		}},
		{DS_StageInfo, {
			{"zoffset", [&]() { setInt(m_vDist); }},
			{"zoffsetlink", [&]() { setInt(m_vDistElemId); }},
			//{"autoturn", [&]() { setInt() }},
			{"resetBG", [&]() { int resetBG; setInt(resetBG); m_resetFlag = resetBG; }},
			{"localcoord", [&]() {
				std::string str;
				setString(str);
				std::stringstream ss(str);
				ss >> m_localCoord[0];
				ss.ignore(1);
				ss >> m_localCoord[1];
			}},
			{"xscale", [&]() { setDouble(m_scale[0]); }},
			{"yscale", [&]() { setDouble(m_scale[1]); }}
		}},
		{DS_Shadow, {
		}},
		{DS_Reflection, {
		}},
		{DS_Music, {
		}},
		{DS_BGDef, {
			{"spr", [&]() { m_spriteLoader.initialize(std::string(folder) + "/" + kv.value()); }},
			{"debugbg", [&]() { int val; setInt(val); m_debugbg = val; }},
			{"type", [&]() {
				std::string type;
				setString(type);
				if (type == "normal")
					currentBgSection = new StaticBgElement;
				else if (type == "animated")
					currentBgSection = new AnimatedBgElement;
				else if (type == "parallax")
					currentBgSection = new ParallaxBgElement;
				currentBgSection->name = newBgSectionName;
			}},
			{"spriteno", [&]() {
				if (StaticBgElement *staticBgElement = dynamic_cast<StaticBgElement *>(currentBgSection)) {
					Spriteref elementSpriteNo;
					std::string str;
					setString(str);
					std::stringstream ss(str);
					ss >> m_localCoord[0];
					ss.ignore(1);
					ss >> m_localCoord[1];
				}
			}},
			{"start", [&]() {  }},
			{"delta", [&]() {  }},
			{"mask", [&]() { }}
		}}
	};
	std::regex bgSection("BG (.*)");
    while ((kv = definitionFile.nextValue())) {
        if (definitionFile.newSection()) {
            if (DefSection != DS_BGDef) {
                std::string section = definitionFile.section();
                std::transform(section.begin(), section.end(), section.begin(), ::tolower);
                if (section == "info")
                    DefSection = DS_Info;
                else if (section == "camera")
                    DefSection = DS_Camera;
                else if (section == "playerinfo")
                    DefSection = DS_PlayerInfo;
                else if (section == "bound")
                    DefSection = DS_Bound;
                else if (section == "stageinfo")
                    DefSection = DS_StageInfo;
                else if (section == "shadow")
                    DefSection = DS_Shadow;
                else if (section == "reflection")
                    DefSection = DS_Reflection;
                else if (section == "music")
                    DefSection = DS_Music;
                else if (section == "bgdef")
                    DefSection = DS_BGDef;
            }
            else {
				// If it's a bgdef sub-section
				std::smatch sm;
				if (std::regex_match(definitionFile.section(), sm, bgSection)) {
					if (currentBgSection) {
						m_bgElements.emplace_back(currentBgSection);
						currentBgSection = nullptr;
					}
					newBgSection = true;
					newBgSectionName = sm[1];
				}
            }
        }
        if (kvEntryExecutor.count(DefSection) && kvEntryExecutor[DefSection].count(kv.name()))
			kvEntryExecutor[DefSection][kv.name()]();
    }
    if (currentBgSection) {
		m_bgElements.emplace_back(currentBgSection);
		currentBgSection = nullptr;
	}
	GlSpriteCollectionBuilder atlasBuilder;
	{
		auto allsprites = m_spriteLoader.load();
		for (auto &bgSection: m_bgElements) {
			if (StaticBgElement *staticElement = dynamic_cast<StaticBgElement *>(bgSection.get())) {
				atlasBuilder.addSprite(allsprites[0].at(staticElement->spriteno).surface());
			}
		}
	}
	m_textureAtlas.reset(atlasBuilder.build());
}

Mugen::SpriteLoader &Stage::spriteLoader()
{
	return m_spriteLoader;
}

void Stage::renderBackground(GlGraphics &glGraphics) {
	GlSpriteDisplayer spriteDisplay(*(m_textureAtlas.get()));
	{
		SDL_Rect bigLoc { 0, 0, glGraphics.window().width(), glGraphics.window().height()};
		spriteDisplay.addSprite(0, bigLoc);
	}
	spriteDisplay.display(glGraphics);
}

}
}
