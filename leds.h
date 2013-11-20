#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ACTION_NORMAL	0
#define ACTION_UP	1
#define ACTION_DOWN	2

#define NUM_MASK	0x01
#define CAPS_MASK	0x02
#define SCROLL_MASK	0x04
#define COMPOSE_MASK	0x08
#define KANA_MASK	0x10

struct led {
	int8_t pin;
	int8_t level;
	uint8_t action;
	bool state;
};

void LED_init();
void LED_set(uint8_t led, bool state);
void LED_handle_sof();
void LED_set_indicators(uint8_t hid_leds);
bool LED_stable(uint8_t led);
bool LED_all_stable();
