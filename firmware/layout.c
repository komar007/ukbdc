#include "layout.h"
#include "aux.h"

#include <avr/pgmspace.h>

/* pointer to program space */
static struct layout_key *data = NULL;
static struct layout_state state = {.last_scancode = NULL};
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

/* Sets current layout */
void LAYOUT_set(struct layout *layout)
{
	state.num_keys = get_pgm_struct_field(layout, num_keys);
	state.num_layers = get_pgm_struct_field(layout, num_layers);
	data = layout->data;
	free(layer_cache);
	layer_cache = malloc(sizeof(*layer_cache) * state.num_keys);
	free(state.last_scancode);
	state.last_scancode = malloc(state.num_keys);
	load_layer(0);
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
