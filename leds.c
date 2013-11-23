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

#include <avr/interrupt.h>

#include "leds.h"
#include "io.h"
#include "platforms.h"
#include "aux.h"
#include "system.h"
#include "timer.h"

void LED_init()
{
	for (int i = 0; i < NUM_LEDS; ++i) {
		if (leds[i].pin == -1)
			continue;
		IO_config(leds[i].pin, OUTPUT);
		IO_set(leds[i].pin, true);
	}

	TCCR0A = 0x00;
	TCCR0B = 0x01;
	TIMSK0 = _BV(TOIE0);
	extern void LED_timer_slow_handler();
	int led_tmr = TIMER_add(256, true);
	SYSTEM_subscribe(TIMER, led_tmr, LED_timer_slow_handler);
}

void LED_set(uint8_t led, bool state)
{
	if (state == leds[led].state)
		return;
	leds[led].state = state;
	if (state) {
		leds[led].action = ACTION_UP;
	} else {
		leds[led].action = ACTION_DOWN;
	}
}

bool LED_stable(uint8_t led)
{
	return leds[led].action == ACTION_NORMAL;
}
bool LED_all_stable()
{
	for (int i = 0; i < NUM_LEDS; ++i) {
		if (leds[i].pin == -1)
			continue;
		if (!LED_stable(i))
			return false;
	}
	return true;
}

void LED_set_indicators(uint8_t hid_leds)
{
	/* FIXME: take led numbers from config, when config API is implemented */
	if (hid_leds & CAPS_MASK)
		LED_set(1, true);
	else
		LED_set(1, false);
}

static const int brightness[] = {0, 1, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64,
	96, 128, 192};

void LED_timer_slow_handler()
{
	for (int i = 0; i < NUM_LEDS; ++i) {
		if (leds[i].pin == -1)
			continue;
		switch (leds[i].action) {
		case ACTION_DOWN:
			if (--leds[i].level < 0) {
				leds[i].level = 0;
				leds[i].action = ACTION_NORMAL;
			}
			break;
		case ACTION_UP:
			if (++leds[i].level > (int)ARR_SZ(brightness) - 1) {
				leds[i].level = ARR_SZ(brightness) - 1;
				leds[i].action = ACTION_NORMAL;
			}
			break;
		default:
			break;
		}
	}
}

void LED_timer_pwm_handler()
{
	static int phase = 0;
	if (phase > 192) {
		phase = 0;
		/* all leds off */
		for (int i = 0; i < NUM_LEDS; ++i) {
			if (leds[i].pin == -1)
				continue;
			IO_set(leds[i].pin, true);
		}
	}
	for (int i = 0; i < NUM_LEDS; ++i) {
		if (leds[i].pin == -1)
			continue;
		if (193 - brightness[leds[i].level] == phase)
			IO_set(leds[i].pin, false);
	}
	++phase;
}

ISR(TIMER0_OVF_vect)
{
	LED_timer_pwm_handler();
}
