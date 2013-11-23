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

#include "platforms.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdbool.h>

#include "usb_keyboard.h"
#include "io.h"
#include "hid.h"
#include "rawhid.h"
#include "rawhid_protocol.h"
#include "dataflash.h"
#include "layout.h"
#include "hc595.h"

#include <stdio.h> /* sprintf */

volatile uint8_t matrix[19][8] = {
	{41, 0, 0, 0, 0, 52, 0, 0},
	{20, 49, 36, 21, 6, 7, 35, 48},
	{0, 0, 58, 0, 0, 0, 0, 0},
	{19, 50, 37, 22, 5, 8, 34, 47},
	{0, 0, 59, 0, 0, 27, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{16, 0, 0, 25, 2, 11, 31, 44},
	{17, 56, 39, 24, 3, 10, 32, 45},
	{14, 0, 0, 40, 0, 13, 29, 42},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{15, 0, 0, 26, 1, 12, 30, 43},
	{18, 51, 38, 23, 4, 9, 33, 46},
	{0, 0, 0, 0, 54, 0, 0, 0},
	{0, 0, 57, 0, 0, 0, 0, 55},
	{0, 0, 0, 60, 28, 0, 53, 0},
};

bool states[19][8] = {{0}};

bool is_modifier(uint8_t key_n)
{
	static const uint8_t modifiers[] = {41, 52, 53, 54, 55, 57, 58, 59, 60};
	for (uint8_t i = 0; i < sizeof(modifiers); ++i)
		if (key_n == modifiers[i])
			return true;
	return false;
}

uint16_t get_input()
{
	uint16_t ret = 0;
	for (int i = 0; i < 16; ++i) {
		ret <<= 1;
		ret |= IO_get(i) ? 0x0001 : 0x0000;
	}
	return ret;
}

void scan_matrix()
{
	bool changed = false;
	for (uint8_t i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j)
			IO_set(j | 0x80, true);
		IO_set((i) | 0x80, false);
		_delay_us(10);
		for (uint8_t j = 0; j < 19; ++j) {
			bool state = !IO_get(j);
			uint8_t layer = 0;
			uint8_t code = LAYOUT_get_scancode(layer, matrix[j][i]);
			if (state != states[j][i]) {
				changed = true;
				states[j][i] = state;
				switch (state) {
				case true:
					if (code == KEY_ESC && (HID_scancode_is_pressed(KEY_LEFT_SHIFT) || HID_scancode_is_pressed(KEY_RIGHT_SHIFT)))
						code = KEY_TILDE;
					HID_set_scancode_state(code, true);
					break;
				case false:
					if (code == KEY_ESC)
						HID_set_scancode_state(KEY_TILDE, false);
					HID_set_scancode_state(code, false);
					break;
				}
			}
		}
	}
	if (changed)
		HID_commit_state();
}

uint8_t xt_to_hid[128] = {
	0x00,
	0x29,
	0x1e,
	0x1f,
	0x20,
	0x21,
	0x22,
	0x23,
	0x24,
	0x25,
	0x26,
	0x27,
	0x2d,
	0x2e,
	0x2a,
	0x2b,
	0x14,
	0x1a,
	0x08,
	0x15,
	0x17,
	0x1c,
	0x18,
	0x0c,
	0x12,
	0x13,
	0x2f,
	0x30,
	0x28,
	0xe0,
	0x04,
	0x16,
	0x07,
	0x09,
	0x0a,
	0x0b,
	0x0d,
	0x0e,
	0x0f,
	0x33,
	0x34,
	0x35,
	0xe1,
	0x31,
	0x1d,
	0x1b,
	0x06,
	0x19,
	0x05,
	0x11,
	0x10,
	0x36,
	0x37,
	0x38,
	0xe5,
	0x55,
	0xe2,
	0x2c,
	0x39,
	0x3a,
	0x3b,
	0x3c,
	0x3d,
	0x3e,
	0x3f,
	0x40,
	0x41,
	0x42,
	0x43,
	0x53,
	0x47,
	0x5f,
	0x60,
	0x61,
	0x56,
	0x5c,
	0x5d,
	0x5e,
	0x57,
	0x59,
	0x5a,
	0x5b,
	0x62,
	0x63,
	0x00,
	0x00,
	0x64,
	0x44,
	0x45,
	0x67,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x68,
	0x69,
	0x6a,
	0x6b,
	0x6c,
	0x6d,
	0x6e,
	0x6f,
	0x70,
	0x71,
	0x72,
	0x00,
	0x88,
	0x00,
	0x00,
	0x87,
	0x00,
	0x00,
	0x73,
	0x00,
	0x00,
	0x8a,
	0x00,
	0x8b,
	0x00,
	0x89,
	0x85,
	0x00
};

int main(void)
{
	clock_prescale_set(clock_div_1);

	for (int i = 0; i < NUM_IO; ++i) {
		IO_config(i, INPUT);
		IO_set(i, true);
	}

	PORTB |= _BV(PB0);
	DDRB |= _BV(PB2) | _BV(PB1) | _BV(PB0); // MOSI, SCK, MEMCS
	DDRC |= _BV(PC7) | _BV(PC6); // LATCH, BUFEN
	DDRB &= ~_BV(PB3); // MISO
	PORTB &= ~_BV(PB3);

#define XT_CLK	8
#define XT_DATA	14

	IO_config(XT_CLK, INPUT);
	IO_config(XT_DATA, INPUT);

	IO_set(XT_CLK, true);
	IO_set(XT_DATA, true);

	USB_init();
	while (USB_get_configuration() == 0)
		;

	HID_commit_state();

	TCCR0A = 0x00;
	TCCR0B = 0x03; /* clk_io / 64 */
	TIMSK0 = _BV(TOIE0);
	while(1) {
		if (IO_get(XT_CLK) == true)
			continue;
		uint16_t recv = 0;
		for (int i = 0; i < 10; ++i) {
			while (IO_get(XT_CLK) == false)
				;
			// rising edge
			if (IO_get(XT_DATA) == true)
				recv |= 1 << i;
			if (i != 9)
				// wait for falling edge
				while (IO_get(XT_CLK) == true)
					;
		}
		// remove start bit
		uint8_t code = recv >> 1;
		HID_set_scancode_state(xt_to_hid[code & 0x7f], !(code >> 7));
		HID_commit_state();
	}

	while (1)
		;
}

void MAIN_handle_sof()
{
}

ISR(TIMER0_OVF_vect)
{
	uint8_t prev_endp = USB_get_endpoint();
	static uint8_t num = 0;
	static bool st = false;
	if (num > 16) {
		num = 0;
		// send keystrokes
	} else {
		++num;
	}
	USB_set_endpoint(prev_endp);
}


