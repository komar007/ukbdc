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

/*! \defgroup LEDS
 * \brief Support for indicator LEDs with soft transitions
 *
 * This module provides a simple interface to control LEDs with soft
 * transitions using software PWM.
 *
 * It also adds one layer of abstraction by assigning all LEDs numbers.
 *
 * This module uses TIMER0 exclusively!
 *
 * @{
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

/*! A structure defining one LED's state */
struct led {
	/*! Pin number (as defined in module IO (io.h)) */
	int8_t pin;
	/*! Current brightness level */
	int8_t level;
	/*! Action currently performed on the LED */
	uint8_t action;
	/*! Current binary state */
	bool state;
};

/*! Initializes LED module. This function should be called before any other
 * function in this module */
void LED_init();
/*! Sets the state of an LED
 * \param led LED number
 * \param state new binary state to set
 */
void LED_set(uint8_t led, bool state);
void LED_set_indicators(uint8_t hid_leds);
/*! Tests if an LED not in the process of changing state
 * \param led LED number
 * \return `true` if the LED is stable, `false` otherwise
 */
bool LED_stable(uint8_t led);
/*! Tests if all LEDs are stable
 * \return `true` if all LEDs are stable, `false` otherwise
 */
bool LED_all_stable();

/*! @} */
