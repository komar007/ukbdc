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

#include "main.h"
#include "usb_keyboard.h"
#include "io.h"
#include "hid.h"
#include "rawhid.h"
#include "rawhid_protocol.h"
#include "layout.h"
#include "matrix.h"

uint8_t matrix[4][5] = {
	{3, 7, 0,  15, 0},
	{2, 6, 10, 14, 0},
	{1, 5, 9,  13, 18},
	{0, 4, 8,  12, 16},
};

uint8_t rows[] = {5, 6, 7, 8};
uint8_t cols[] = {0, 1, 2, 3, 4};

#define LED 9

volatile bool should_scan = false;

int main(void)
{
	clock_prescale_set(clock_div_1);

	IO_config(LED, OUTPUT);
	IO_set(LED, true);

	USB_init();
	while (USB_get_configuration() == 0)
		;

	/* initialize with 20 keys */
	LAYOUT_init(20);

	int ret = LAYOUT_set((const struct layout*)LAYOUT_BEGIN);
	if (ret != 0) {
		for (int i = 0; i < 30; ++i) {
			IO_set(LED, false);
			_delay_ms(200);
			IO_set(LED, true);
			_delay_ms(200);
		}
	}

	HID_commit_state();
	LAYOUT_set_callback(&HID_set_scancode_state);

	MATRIX_init(4, rows, 5, cols, (const uint8_t*)matrix, &LAYOUT_set_key_state);

	TCCR0A = 0x00;
	TCCR0B = 0x03; /* clk_io / 64 */
	TIMSK0 = _BV(TOIE0);
	while(1) {
		if (should_scan) {
			should_scan = false;
			bool changed = MATRIX_scan();
			if (changed)
				HID_commit_state();
			/*if (HID_get_leds() & 0x02)
				IO_set(LED, false);
			else
				IO_set(LED, true);*/
		}
		RAWHID_PROTOCOL_task();
	}

	while (1)
		;
}

void MAIN_handle_sof()
{
}

ISR(TIMER0_OVF_vect)
{
	static uint8_t num = 0;
	if (num > 16) {
		num = 0;
		should_scan = true;
	} else {
		++num;
	}
}
