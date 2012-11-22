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
#include "layout.h"
#include "matrix.h"

#include <stdio.h> /* sprintf */

volatile uint8_t matrix[5][14] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
	{14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
	{28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41},
	{42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55},
	{56, 57, 58, 0, 0, 59, 0, 0, 0, 0, 60, 61, 62, 63},
};

uint8_t rows[] = {0, 1, 2, 3, 4};
uint8_t cols[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

uint8_t PROGMEM scan_codes[2][64] = {
	{KEY_ESC,  KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE,
	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_BACKSLASH,
	KEY_CAPS_LOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE, KEY_BACKSLASH, KEY_ENTER,
	KEY_LEFT_SHIFT, 0, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, 0, KEY_RIGHT_SHIFT,
	KEY_LEFT_CTRL, 0, KEY_LEFT_ALT, KEY_SPACE, KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_RIGHT_GUI, KEY_RIGHT_CTRL},

	{KEY_TILDE,  KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_DELETE,
	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_INSERT, KEY_O, KEY_P, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_BACKSLASH,
	KEY_CAPS_LOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_LEFT, KEY_DOWN, KEY_UP, KEY_RIGHT, KEY_SEMICOLON, KEY_QUOTE, KEY_BACKSLASH, KEY_ENTER,
	KEY_LEFT_SHIFT, 0, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, 0, KEY_RIGHT_SHIFT,
	KEY_LEFT_CTRL, 0, KEY_LEFT_ALT, KEY_SPACE, KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_RIGHT_GUI, KEY_RIGHT_CTRL}
};

bool states[5][14] = {{0}};

bool is_modifier(uint8_t key_n)
{
	static const uint8_t modifiers[] = {41, 52, 53, 54, 55, 57, 58, 59, 60};
	for (uint8_t i = 0; i < sizeof(modifiers); ++i)
		if (key_n == modifiers[i])
			return true;
	return false;
}

uint8_t layer = 0;
void scan_callback(uint8_t num, bool state)
{
	uint8_t code = LAYOUT_get_scancode(layer, num);
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

int main(void)
{
	clock_prescale_set(clock_div_1);

	for (int i = 5; i < NUM_IO; ++i) {
		IO_config(i, INPUT);
		IO_set(i, true);
	}
	for (int i = 0; i < 5; ++i) {
		IO_config(i, INPUT);
		IO_set(i, false); // no pull-up
	}

	uint8_t *layout = malloc(sizeof(scan_codes));
	for (unsigned i = 0; i < sizeof(scan_codes); ++i)
		layout[i] = pgm_read_byte((void*)scan_codes+i);
	LAYOUT_set(layout);

	MATRIX_init(5, rows, 14, cols, (const uint8_t*)matrix, &scan_callback);

	USB_init();
	while (USB_get_configuration() == 0)
		;

	HID_commit_state();

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
		bool changed = MATRIX_scan();
		if (changed)
			HID_commit_state();
	} else {
		++num;
	}
	USB_set_endpoint(prev_endp);
}
