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

struct cns_section_t {
};

/**
 * \brief Structure for a state definition, with multiple sections.
 */
struct cns_state_t {
	/**
	* \brief State definition type.
	*/
	enum cns_state_type_t: char {
		STATETYPE_S = 'S', /**< Standing. */
		STATETYPE_C = 'C', /**< Crouching. */
		STATETYPE_A = 'A', /**< Air. */
		STATETYPE_L = 'L', /**< Lying down. */
		STATETYPE_U = 'U' /**< Unchanged from previous state. */
	};
	/**
	* \brief Move type.
	*/
	enum cns_state_movetype_t: char {
		MOVETYPE_A = 'A', /**< Attack. */
		MOVETYPE_I = 'I', /**< Idle. */
		MOVETYPE_H = 'H', /**< Being hit. */
		MOVETYPE_U = 'U' /**< Unchanged from previous state. */
	};
	/**
	* \brief Physics to use for the state.
	*/
	enum cns_state_physics_t: char {
		PHYSICS_S = 'S', /**< Standing. */
		PHYSICS_C = 'C', /**< Crouching. */
		PHYSICS_A = 'A', /**< Air. */
		PHYSICS_N = 'N', /**< None. */
		PHYSICS_U = 'U' /**< Unchanged from previous state. */
	};
	/**
	 * \brief Whether P1 is in control.
	 */
	enum cns_state_ctrl_t {
		CTRL_FALSE = 0, /**< P1 doesn't have control.*/
		CTRL_TRUE = 1, /**< P1 has control.*/
		CTRL_UNCHANGED /**< Unchanged from previous state.*/
	};
	/**
	 * @name Basic state parameters
	 * @{
	 */
	/**
	* \brief State definition type.
	* Default value: S.
	*/
	cns_state_type_t type = STATETYPE_S;
	/**
	 * \brief Move type.
	 * Default value: I.
	 */
	cns_state_movetype_t movetype = MOVETYPE_I;
	/**
	 * \brief Physics to use for the state.
	 * Default value: N.
	 */
	cns_state_physics_t physics = PHYSICS_N;
	/**
	 * \brief Identifier of the corresponding animation (defined in the AIR file of the character).
	 */
	unsigned int anim;
	/**
	 * @}
	 */
	/**
	 * @name Additional state parameters
	 * @{
	 */
	/**
	 * \brief P1's velocity at the beginning of the state.
	 * Note: even if it's set to 0, attacking P2 in the corner will push P1 away.
	 */
	int velset[2] = {0, 0};
	/**
	 * \brief Number (positive or negative) added to the player's power bar.
	 */
	int poweradd = 0;
	/**
	 * @}
	 */
	/**
	 * \brief State sections
	 */
	std::vector<cns_section_t> s;
};

}

#endif // CNS_H