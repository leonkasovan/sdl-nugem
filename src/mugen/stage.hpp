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

#ifndef STAGE_HPP
#define STAGE_HPP

#include "sprites.hpp"
#include "def.hpp"
#include "../glgraphics.hpp"
#include "../glsprite.hpp"

#include <memory>

namespace Nugem {
namespace Mugen {

class Stage {	
public:
	Stage(const std::string &);
	void initialize();
	Mugen::SpriteLoader &spriteLoader();
	void renderBackground(GlGraphics &glGraphics);
private:
	constexpr static const char *folder = "stages";
	const std::string m_loadingName;
	// ============================
	// Definition file values
	// ============================
	// Info group
	std::string m_name;
	std::string m_displayName;
	std::string m_versionDate;
	std::string m_mugenVersion;
	std::string m_author;
	// Camera group
	/** Starting x and y-position for the camera */
	int m_start[2];
	/** Minimum x-position for the camera */
	int m_boundleft;
	/** Maximum x-position for the camera */
	int m_boundright;
	/** Minimum y-position for the camera */
	int m_boundhigh;
	/** Maximum y-position for the camera */
	int m_boundlow;
	/** Horizontal distance that a player can get from the left / right edes before the camera starts to move to follow */
	int m_tension;
	int m_tensionhigh;
	int m_tensionlow;
	double m_verticalfollow;
	int m_floortension;
	int m_overdrawhigh;
	int m_overdrawlow;
	int m_cuthigh;
	int m_cutlow;
	double m_startzoom;
	double m_zoomout;
	double m_zoomin;
	// Player group
	int m_p1start[2];
	int m_p2start[2];
	bool m_p1facing;
	bool m_p2facing;
	int m_playerleftbound;
	int m_playerrightbound;
	// Bound group
	int m_screenleft;
	int m_screenright;
	// StageInfo group
	int m_vDist = 0;
	int m_vDistElemId = -1;
	bool m_resetFlag;
	int m_localCoord[2];
	double m_scale[2];
	// Shadow group
	int m_darkness;
	// Reflection group
	// Music group
	std::string m_bgmusic;
	int m_bgvolume;
	// Background definition elements
    	Mugen::SpriteLoader m_spriteLoader;
	bool m_debugbg;
	struct BgElement {
		std::string name;
		virtual ~BgElement() {};
	};
	struct StaticBgElement: public BgElement {
        Spriteref spriteref;
		int layer = 0;
        double start[2] = {0.0f, 0.0f};
        double delta[2] = {1.0f, 1.0f};
		bool mask;
		int tile[2] = {0, 0};
		int tilespacing[2] = {0, 0};
        size_t atlasid;
	};
	struct AnimatedBgElement: public BgElement {};
	struct ParallaxBgElement: public BgElement {};
	std::vector<std::unique_ptr<BgElement>> m_bgElements;
	std::unique_ptr<GlSpriteCollection> m_textureAtlas;

	// Camera position
	int m_camera[2];
};

}
}

#endif
