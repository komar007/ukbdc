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

#include "layout.h"
#include "auxiliary.h"

#include <avr/pgmspace.h>

/* pointer to program space */
static const struct layout_key *data = NULL;
static struct layout_state state = {
	.last_scancode = NULL,
	.active = false
};
static struct layout_key *layer_cache = NULL;
static scancode_callback_t scancode_callback = NULL;

void load_layer(uint8_t num)
{
	for (int i = 0; i < state.num_keys; ++i) {
		uint32_t dword = pgm_read_dword(data + num*state.num_keys + i);
		layer_cache[i] = *(struct layout_key*)&dword;
	}
	state.cur_layer = num;
}

int LAYOUT_init(int num_keys)
{
	state.num_keys = num_keys;
	layer_cache = malloc(sizeof(*layer_cache) * num_keys);
	state.last_scancode = malloc(num_keys);
	return 0;
}

/* Sets current layout */
int LAYOUT_set(const struct layout *layout)
{
	uint8_t num_keys = get_pgm_struct_field(layout, num_keys);
	if (num_keys != state.num_keys)
		return -1;
	state.num_layers = get_pgm_struct_field(layout, num_layers);
	data = layout->data;
	load_layer(0);
	state.active = true;
	return 0;
}

void LAYOUT_deactivate()
{
	state.active = false;
}

/* Sets the function which will be called each time a scancode should be
 * emitted */
void LAYOUT_set_callback(scancode_callback_t callback)
{
	scancode_callback = callback;
}

/* Sends a key press or key release event to layout */
void LAYOUT_set_key_state(uint8_t key, bool event)
{
	if (!state.active)
		return;
	if (event == DOWN) {
		if (layer_cache[key].scode != 0) {
			state.last_scancode[key] = layer_cache[key].scode;
			scancode_callback(layer_cache[key].scode, DOWN);
		}
		switch (layer_cache[key].actions >> 4) {
		case REL:
			load_layer(state.cur_layer +
					(int8_t)layer_cache[key].down_arg);
			break;
		case ABS:
			load_layer(layer_cache[key].down_arg);
			break;
		default:
			break;
		}
	} else {
		if (layer_cache[key].scode != 0)
			scancode_callback(state.last_scancode[key], UP);
		switch (layer_cache[key].actions & 0x0f) {
		case REL:
			load_layer(state.cur_layer +
					(int8_t)layer_cache[key].up_arg);
			break;
		case ABS:
			load_layer(layer_cache[key].up_arg);
			break;
		default:
			break;
		}
	}
}
