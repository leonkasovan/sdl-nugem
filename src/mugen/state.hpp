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

#ifndef STATE_HPP
#define STATE_HPP

#include <vector>

namespace Nugem {
namespace Mugen {

/**
 * 
 */

/**
 * \brief Controller for a state, read from a CNS file.
 * 
 * The attributes of a controller are parameters defined in the controller block, starting with the line <tt>[Statedef \ref statenumber, \ref controllernumber]</tt>.
 */
struct cns_section_t {
	/**
	 *\brief Controller number.
	 * 
	 * This can be just any number, it is only reported when an error is found.
	 */
	unsigned int controllernumber;

	
};

/**
 * \brief Structure for a single state read from a CNS file.
 * 
 * All the structure's attributes, except for the state number and the state controllers vector \b s, are parameters defined in the state definition <tt>[Statedef \ref statenumber]</tt>. A state must have at least one state controller. 
 * \a P1 here designates the player. \a P2 designates the player's opponent.
 */
struct cns_state_t {
	/** \brief State definition type. */
	enum cns_state_type_t: char {
		STATETYPE_S = 'S', /**< Standing */
		STATETYPE_C = 'C', /**< Crouching */
		STATETYPE_A = 'A', /**< Air */
		STATETYPE_L = 'L', /**< Lying down */
		STATETYPE_U = 'U' /**< Unchanged from previous state */
	};
	/** \brief Move type. */
	enum cns_state_movetype_t: char {
		MOVETYPE_A = 'A', /**< Attack */
		MOVETYPE_I = 'I', /**< Idle */
		MOVETYPE_H = 'H', /**< Being hit */
		MOVETYPE_U = 'U' /**< Unchanged from previous state */
	};
	/** \brief Physics to use for the state. */
	enum cns_state_physics_t: char {
		PHYSICS_S = 'S', /**< Standing */
		PHYSICS_C = 'C', /**< Crouching */
		PHYSICS_A = 'A', /**< Air */
		PHYSICS_N = 'N', /**< None */
		PHYSICS_U = 'U' /**< Unchanged from previous state */
	};
	/** \brief Whether P1 is in control or not in control. */
	enum cns_state_ctrl_t {
		CTRL_FALSE = 0, /**< P1 doesn't have control */
		CTRL_TRUE = 1, /**< P1 has control */
		CTRL_UNCHANGED /**< Unchanged from previous state */
	};
	/** \brief State number. */
	unsigned int statenumber;
	/** \brief State sections */
	std::vector<cns_section_t> s;
	/**
	 * @name Basic state parameters
	 * @{
	 */
	/** \brief State definition type. */
	cns_state_type_t type = STATETYPE_S;
	/** \brief Move type. */
	cns_state_movetype_t movetype = MOVETYPE_I;
	/** \brief Physics to use for the state. */
	cns_state_physics_t physics = PHYSICS_N;
	/** \brief Identifier of the corresponding animation (defined in the AIR file of the character). */
	unsigned int anim;
	/**
	 * @}
	 */
	/**
	 * @name Additional state parameters
	 * @{
	 */
	/**
	 * \brief Velocity of P1 at the beginning of the state.
	 * 
	 * However, even if it is set to 0, attacking P2 in the corner will push P1 away.
	 */
	int velset[2] = {0, 0};
	/** \brief Number (positive or negative) added to the player's power bar. */
	int poweradd = 0;
	/**
	 * \brief How many points of juggling the move requires.
	 * This parameter is only useful for attacks, but it should be included for all attacks.
	 * 
	 * If negative, the attack will juggle if the previous attacking state successfully juggled.
	 * If an attack spans more than one state, this parameter should only be included for the first state.
	 */
	int juggle = -1;
	/** \brief If set to true, P1 will be turned to face P2 at the beginning of the state. */
	bool facep2 = false;
	/** \brief If set to true, any HitDefs active at the time of a state transition to this state will remain active. If false, any such HitDefs will be disabled instead. */
	bool hitdefpersist = false;
	/** \brief If set to true, the move hit information from the previous state will be carried over into this state. If false, the move hit information will be reset. */
	bool movehitpersist = false;
	/** \brief If set to true, the hit counter will be carried over from the previous state. If false, it will be reset at the start of this state. */
	bool hitcountpersist = false;
	/** \brief Sprite layering priority for P1. If negative, the sprite priority will be left unchanged. */
	int sprpriority = -1;
	/**
	 * @}
	 */
};

}
}

#endif // STATE_HPP
