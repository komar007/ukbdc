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
