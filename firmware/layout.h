#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define NONE	0x00
#define REL	0x01
#define ABS	0x02

#define DOWN	1
#define UP	0

#define ACT(dir, type) ((type) << (4*(dir)))

struct layout_key {
	/* scan code to send
	 * 0 means no scancode */
	uint8_t scode;
	/* low 4 bits:  action type on key-down event
	 * high 4 bits: action type on key-up event */
	uint8_t actions;
	/* arguments to down- and up-action, respectively */
	uint8_t down_arg;
	uint8_t up_arg;
};

struct layout {
	uint8_t num_keys;
	uint8_t num_layers;
	/* pointer to program space */
	struct layout_key data[];
};

struct layout_state {
	bool active;
	int num_keys;
	int num_layers;
	int cur_layer;
	/* last scancode sent by each key
	 * this is to make sure a scancode is released even if a key is
	 * released on a different layer */
	uint8_t *last_scancode;
};

typedef void (*scancode_callback_t)(uint8_t code, bool state);

void LAYOUT_init(int num_keys);
int LAYOUT_set(struct layout *layout);
void LAYOUT_set_callback(scancode_callback_t callback);
void LAYOUT_set_key_state(uint8_t key, bool state);
void LAYOUT_deactivate();
