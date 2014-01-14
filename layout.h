/* This file is part of ukbdc.
 *
 * ukbdc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ukbdc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ukbdc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*! \defgroup LAYOUT
 * \brief Support for multi-layered keyboard layouts
 *
 * This module provides support for keyboard layouts with layers and layer
 * changing actions.
 *
 * It is responsible to convert information about key state changes to
 * scancodes which should be sent to the PC. The hardware keys are identified
 * by numbers, as returned by the MATRIX (matrix.h) module.
 *
 * The information about pressed keys is provided to the module using
 * LAYOUT_set_key_state(). The module handles key actions defined in the
 * binary layout description and changes the layers accordingly which may
 * result in different scancodes being generated for the same keys, as the
 * scancodes depend on the current layer, which can change.
 *
 * The information when scancodes should be changed is provided using one
 * simple callback.
 *
 * @{
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*! no action assigned to this key */
#define NONE	0x00
/*! relative action */
#define REL	0x01
/*! absolute action */
#define ABS	0x02

/*! when pressed down */
#define DOWN	1
/*! when released */
#define UP	0

#define ACT(dir, type) ((type) << (4*(dir)))

/*! A description of a single key on a single layer */
struct layout_key {
	/*! Scan code to send
	 * 0 means no scancode */
	uint8_t scode;
	/*! Actions to perform on a key.
	 * Low 4 bits - action type on key-down event.
	 * High 4 bits - action type on key-up event. */
	uint8_t actions;
	/*! Argument to down action */
	uint8_t down_arg;
	/*! Argument to up action */
	uint8_t up_arg;
};

/*! A description of a keyboard layout (with a numbe of layers) */
struct layout {
	/*! Number of keys */
	uint8_t num_keys;
	/*! Number of layers */
	uint8_t num_layers;
	/*! Pointer to program space where the layers are stored */
	struct layout_key data[];
};

/*! A structure which stores the layout's state */
struct layout_state {
	/*! Indicates whether the layout generates any actions or keypresses */
	bool active;
	/*! Number of keys */
	uint8_t num_keys;
	/*! Number of layers */
	uint8_t num_layers;
	/*! Number of currently chosen layer */
	uint8_t cur_layer;
	/*! The last scancode sent by each key.
	 * This is to make sure a scancode is released even if a key is
	 * released on a different layer. */
	uint8_t *last_scancode;
};

/*! Type of function called each time a scancode's state should be changed
 * \param code the scancode
 * \param state the new state of the scancode
 */
typedef void (*scancode_callback_t)(uint8_t code, bool state);

/*! Initializes the LAYOUT module. This function should be called before any
 * other function in this module
 * \param num_keys number of keys on a layer
 * \return `0` on success, other value on error
 */
int LAYOUT_init(int num_keys);
/*! Sets the layout binary description to be used
 * \param layout a pointer to program space where the layout description
 * begins
 * \return `0` on success, other value on error
 */
int LAYOUT_set(const struct layout *layout);
/*! Sets the callback which will be called each time an actual scancode should
 * be changed based on the state of the keys */
void LAYOUT_set_callback(scancode_callback_t callback);
/*! Informs the LAYOUT module of a key's state change
 * \param key key number
 * \param event either \ref UP or \ref DOWN, indicating what happened to the
 * key (equivalent to the new state of the key)
 */
void LAYOUT_set_key_state(uint8_t key, bool event);
void LAYOUT_deactivate();

/*! @} */
