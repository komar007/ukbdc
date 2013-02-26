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


	USB_init();
	while (USB_get_configuration() == 0)
		;

	HID_commit_state();

	int size;
	DATAFLASH_read_page(1, sizeof(size), &size);
	if (size != -1) {
		uint8_t *layout = malloc(size);
		DATAFLASH_read_page(2, (uint16_t)size, layout);
		RAWHID_send(layout);
		LAYOUT_set(layout);
	}
	DATAFLASH_read_page(0, sizeof(matrix), matrix);

	TCCR0A = 0x00;
	TCCR0B = 0x03; /* clk_io / 64 */
	TIMSK0 = _BV(TOIE0);
	while(1) {
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
	uint8_t prev_endp = USB_get_endpoint();
	static uint8_t num = 0;
	if (num > 16) {
		num = 0;
		scan_matrix();
	} else {
		++num;
	}
	USB_set_endpoint(prev_endp);
}


