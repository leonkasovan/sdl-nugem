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


enum class DefSection {
	None,
	Info,
	Camera,
	PlayerInfo,
	Bound,
	StageInfo,
	Shadow,
	Reflection,
	Music,
	BGDef
};

void Stage::initialize() {
    MugenTextFile definitionFile(std::string(folder) + "/" + m_loadingName + ".def");
    MugenTextKeyValue kv;
	DefSection defSection = DefSection::None;
    auto setString = [&](std::string &result) {
        result = kv.value();
    };
    auto setInt = [&](int &result) {
        result = std::stoi(kv.value());
    };
    auto setDouble = [&](double &result) {
        result = std::stod(kv.value());
    };
    std::string newBgSectionName = "";
    std::unique_ptr<BgElement> currentBgSection;
    std::unordered_map<DefSection,std::unordered_map<std::string, std::function<void(void)>>> kvEntryExecutor {
        {   DefSection::Info, {
                {"name", [&]() {
                        setString(m_name);
                    }
                },
                {"displayname", [&]() {
                        setString(m_displayName);
                    }
                },
                {"versiondate", [&]() {
                        setString(m_versionDate);
                    }
                },
                {"mugenversion", [&]() {
                        setString(m_mugenVersion);
                    }
                },
                {"author", [&]() {
                        setString(m_author);
                    }
                }
            }
        },
        {   DefSection::Camera, {
                {"startx", [&]() {
                        setInt(m_start[0]);
                    }
                },
                {"starty", [&]() {
                        setInt(m_start[1]);
                    }
                },
                {"boundleft", [&]() {
                        setInt(m_boundleft);
                    }
                },
                {"boundright", [&]() {
                        setInt(m_boundright);
                    }
                },
                {"boundhigh", [&]() {
                        setInt(m_boundhigh);
                    }
                },
                {"boundlow", [&]() {
                        setInt(m_boundlow);
                    }
                },
                {"tension", [&]() {
                        setInt(m_tension);
                    }
                },
                {"tensionhigh", [&]() {
                        setInt(m_tensionhigh);
                    }
                },
                {"tensionlow", [&]() {
                        setInt(m_tensionlow);
                    }
                },
                {"verticalfollow", [&]() {
                        setDouble(m_verticalfollow);
                    }
                },
                {"floortension", [&]() {
                        setInt(m_floortension);
                    }
                },
                {"overdrawhigh", [&]() {
                        setInt(m_overdrawhigh);
                    }
                },
                {"overdrawlow", [&]() {
                        setInt(m_overdrawlow);
                    }
                },
                {"cuthigh", [&]() {
                        setInt(m_cuthigh);
                    }
                },
                {"cutlow", [&]() {
                        setInt(m_cutlow);
                    }
                },
                {"startzoom", [&]() {
                        setDouble(m_startzoom);
                    }
                },
                {"zoomout", [&]() {
                        setDouble(m_zoomout);
                    }
                },
                {"zoomin", [&]() {
                        setDouble(m_zoomin);
                    }
                },
            }
        },
        {   DefSection::PlayerInfo, {
                {"p1startx", [&]() {
                        setInt(m_p1start[0]);
                    }
                },
                {"p1starty", [&]() {
                        setInt(m_p1start[1]);
                    }
                },
                {"p2startx", [&]() {
                        setInt(m_p2start[0]);
                    }
                },
                {"p2starty", [&]() {
                        setInt(m_p2start[1]);
                    }
                },
                {"p1facing", [&]() {
                        int val;
                        setInt(val);
                        m_p1facing = (val > 0);
                    }
                },
                {"p2facing", [&]() {
                        int val;
                        setInt(val);
                        m_p2facing = (val > 0);
                    }
                },
                {"leftbound", [&]() {
                        setInt(m_playerleftbound);
                    }
                },
                {"rightbound", [&]() {
                        setInt(m_playerrightbound);
                    }
                }
            }
        },
        {   DefSection::Bound, {
                {"screenleft", [&]() {
                        setInt(m_screenleft);
                    }
                },
                {"screeenright", [&]() {
                        setInt(m_screenright);
                    }
                }
            }
        },
        {   DefSection::StageInfo, {
                {"zoffset", [&]() {
                        setInt(m_vDist);
                    }
                },
                {"zoffsetlink", [&]() {
                        setInt(m_vDistElemId);
                    }
                },
                //{"autoturn", [&]() { setInt() }},
                {"resetBG", [&]() {
                        int resetBG;
                        setInt(resetBG);
                        m_resetFlag = resetBG;
                    }
                },
                {   "localcoord", [&]() {
                        std::string str;
                        setString(str);
                        std::stringstream ss(str);
                        ss >> m_localCoord[0];
                        ss.ignore(1);
                        ss >> m_localCoord[1];
                    }
                },
                {"xscale", [&]() {
                        setDouble(m_scale[0]);
                    }
                },
                {"yscale", [&]() {
                        setDouble(m_scale[1]);
                    }
                }
            }
        },
        {   DefSection::Shadow, {
            }
        },
        {   DefSection::Reflection, {
            }
        },
        {   DefSection::Music, {
            }
        },
        {   DefSection::BGDef, {
                {   "spr", [&]() {
                        m_spriteLoader.initialize(std::string(folder) + "/" + kv.value());
                    }
                },
                {"debugbg", [&]() {
                        int val;
                        setInt(val);
                        m_debugbg = val;
                    }
                },
                {   "type", [&]() {
                        std::string type;
                        setString(type);
                        if (type == "normal")
                            currentBgSection.reset(new StaticBgElement);
                        else if (type == "animated")
                            currentBgSection.reset(new AnimatedBgElement);
                        else if (type == "parallax")
                            currentBgSection.reset(new ParallaxBgElement);
                        currentBgSection->name = newBgSectionName;
                    }
                },
                {   "spriteno", [&]() {
                        if (StaticBgElement *staticBgElement = dynamic_cast<StaticBgElement *>(currentBgSection.get())) {
                            std::string str;
                            setString(str);
                            std::stringstream ss(str);
                            ss >> staticBgElement->spriteref.group;
                            ss.ignore(1);
                            ss >> staticBgElement->spriteref.image;
                        }
                    }
                },
                {   "layerno", [&]() {
                        if (StaticBgElement *staticBgElement = dynamic_cast<StaticBgElement *>(currentBgSection.get())) {
                            setInt(staticBgElement->layer);
                        }
                    }
                },
                {   "start", [&]() {
                        if (StaticBgElement *staticBgElement = dynamic_cast<StaticBgElement *>(currentBgSection.get())) {
                            std::string str;
                            setString(str);
                            std::stringstream ss(str);
                            ss >> staticBgElement->start[0];
                            ss.ignore(1);
                            ss >> staticBgElement->start[0];
                        }
                    }
                },
                {   "delta", [&]() {
                        if (StaticBgElement *staticBgElement = dynamic_cast<StaticBgElement *>(currentBgSection.get())) {
                            std::string str;
                            setString(str);
                            std::stringstream ss(str);
                            ss >> staticBgElement->delta[0];
                            ss.ignore(1);
                            ss >> staticBgElement->delta[0];
                        }
                    }
                },
                {   "mask", [&]() {
                        if (StaticBgElement *staticBgElement = dynamic_cast<StaticBgElement *>(currentBgSection.get())) {
                            int val;
                            setInt(val);
                            staticBgElement->mask = val;
                        }
                    }
                }
            }
        }
    };
    std::regex bgSection("BG (.*)");
    std::unordered_map<std::string, DefSection> sectionMap = {
        {"info", DefSection::Info},
        {"camera", DefSection::Camera},
        {"playerinfo", DefSection::PlayerInfo},
        {"bound", DefSection::Bound},
        {"stageinfo", DefSection::StageInfo},
        {"shadow", DefSection::Shadow},
        {"reflection", DefSection::Reflection},
        {"music", DefSection::Music},
        {"bgdef", DefSection::BGDef}
    };
    while ((kv = definitionFile.nextValue())) {
        if (definitionFile.newSection()) {
            if (defSection != DefSection::BGDef) {
                std::string section = definitionFile.section();
                std::transform(section.begin(), section.end(), section.begin(), ::tolower);
                if (sectionMap.count(section))
                    defSection = sectionMap.at(section);
                else
                    defSection = DefSection::None;
            }
            else {
                // If it's a bgdef sub-section
                std::smatch sm;
                if (std::regex_match(definitionFile.section(), sm, bgSection)) {
                    if (currentBgSection)
                        m_bgElements.emplace_back(currentBgSection.release());
                    newBgSectionName = sm[1];
                }
            }
        }
        if (kvEntryExecutor[defSection].count(kv.name()))
            kvEntryExecutor[defSection][kv.name()]();
    }
    if (currentBgSection)
        m_bgElements.emplace_back(currentBgSection.release());
    GlSpriteCollectionBuilder atlasBuilder;
    {
        auto allsprites = m_spriteLoader.load();
        for (auto &bgSection: m_bgElements) {
            if (StaticBgElement *staticElement = dynamic_cast<StaticBgElement *>(bgSection.get())) {
                staticElement->atlasid = atlasBuilder.addSprite(allsprites[0].at(staticElement->spriteref).surface());
            }
        }
    }
    m_textureAtlas.reset(atlasBuilder.build());
    m_camera[0] = m_start[0];
    m_camera[1] = m_start[0];
}

Mugen::SpriteLoader &Stage::spriteLoader()
{
    return m_spriteLoader;
}

void Stage::renderBackground(GlGraphics &glGraphics) {
	if (m_textureAtlas) {
		GlSpriteDisplayer spriteDisplay(*(m_textureAtlas.get()));
// 		SDL_Rect totalScreen { 0, 0, static_cast<int>(glGraphics.window().width()), static_cast<int>(glGraphics.window().height()) };
		for (auto &bgSection: m_bgElements) {
			SDL_Rect currentPosition;
			if (StaticBgElement *staticElement = dynamic_cast<StaticBgElement *>(bgSection.get())) {
				currentPosition.x = staticElement->start[0];
				currentPosition.y = staticElement->start[1];
				auto &spr = m_textureAtlas->sprites()[staticElement->atlasid];
				currentPosition.w = spr.w;
				currentPosition.h = spr.h;
				spriteDisplay.addSprite(staticElement->atlasid, currentPosition);
			}
		}
		spriteDisplay.display(glGraphics);
	}
}

}
}
