/*
 * Copyright (C) Victor Nivet
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

#ifndef CNS_H
#define CNS_H

#include <vector>

namespace mugen {

struct cns_state_section_t {
};

struct cns_statedef_t {
	/**
	* \brief State type of the player
	*/
	enum type {
		S = 'S', /**< Standing */
		C = 'C', /**< Crouching */
		A = 'A', /**< Air */
		L = 'L', /**< Lying down */
		U = 'U' /**< Unchanged from previous state */
	};
};

struct cns_state_t {
	cns_statedef_t statedef;
	std::vector<cns_statedef_t> sections;
};

}

#endif // CNS_H