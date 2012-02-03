#include "platforms.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdbool.h>

#include "usb_keyboard.h"
#include "hid.h"
#include "rawhid.h"
#include "rawhid_protocol.h"
#include "dataflash.h"
#include "layout.h"
#include "hc595.h"

uint8_t number_keys[10]=
	{KEY_0,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9};

uint8_t matrix[8][8] = {
	{23,  0, 20, 24, 15, 19, 16,  0},
	{50, 52, 49, 51, 43, 46, 45, 41},
	{36, 27, 48, 39, 30, 33, 44, 28},
	{58, 60, 57, 59, 55, 47, 56, 42},
	{22, 26, 21, 25, 14, 18, 17, 53},
	{37, 40, 35, 38, 31, 34, 32, 29},
	{ 8, 12,  7, 11,  0,  4,  3, 54},
	{ 9, 13,  6, 10,  1,  5,  2,  0},
};

bool states[8][8] = {{0}};

uint8_t PROGMEM scan_codes[2][61] = {
	{KEY_ESC,  KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_BACKSPACE,
	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_BACKSLASH,
	KEY_CAPS_LOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE, KEY_ENTER,
	KEY_LEFT_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_SHIFT,
	KEY_LEFT_CTRL, 0, KEY_LEFT_ALT, KEY_SPACE, KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_RIGHT_GUI, KEY_RIGHT_CTRL},

	{KEY_TILDE,  KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_DELETE,
	KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_INSERT, KEY_O, KEY_P, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_BACKSLASH,
	KEY_CAPS_LOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_LEFT, KEY_DOWN, KEY_UP, KEY_RIGHT, KEY_SEMICOLON, KEY_QUOTE, KEY_ENTER,
	KEY_LEFT_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, KEY_RIGHT_SHIFT,
	KEY_LEFT_CTRL, 0, KEY_LEFT_ALT, KEY_SPACE, KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_RIGHT_GUI, KEY_RIGHT_CTRL}
};

bool is_modifier(uint8_t key_n)
{
	static const uint8_t modifiers[] = {41, 52, 53, 54, 55, 57, 58, 59, 60};
	for (uint8_t i = 0; i < sizeof(modifiers); ++i)
		if (key_n == modifiers[i])
			return true;
	return false;
}

void scan_matrix()
{
	bool changed = false;
	for (uint8_t i = 0; i < 8; ++i) {
		PORTD = ~(0x01 << i);
		_delay_us(10);
		uint8_t val = PINB;
		for (uint8_t j = 0; j < 8; ++j) {
			bool state = !(val & 0x01);
			uint8_t layer = states[6][7];
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
			val >>= 1;
		}
	}
	if (changed)
		HID_commit_state();
	PORTD = 0xff;
}

int main(void)
{
	clock_prescale_set(clock_div_1);
	/* PORTB - input, pull up */
	PORTB = 0xff;
	DDRB  = 0x00;

#ifdef PLATFORM_alpha
	DDRB |= _BV(PB2) | _BV(PB1) | _BV(PB0);
	PORTB &= ~(_BV(PB1) | _BV(PB3));

	PORTC &= ~(_BV(PC6) | _BV(PC7));
	DDRC |= _BV(PC6) | _BV(PC7);
#endif
	/* PORTD - output */
	PORTD = 0xff;
	DDRD = 0xff;

	USB_init();
	while (USB_get_configuration() == 0)
		;

	HID_commit_state();
	uint8_t buf[518] = "kupka kupka\n";

#ifdef PLATFORM_alpha
	DATAFLASH_read_page(0, buf);
	HC595_set_outputs(0xaaaa);
#endif

	uint8_t *layout = malloc(sizeof(scan_codes));
	for (int i = 0; i < sizeof(scan_codes); ++i)
		layout[i] = pgm_read_byte((void*)scan_codes+i);
	LAYOUT_set(layout);

	TCCR0A = 0x00;
	TCCR0B = 0x03; /* clk_io / 64 */
	TIMSK0 = _BV(TOIE0);
	while(1) {
		RAWHID_task();
		RAWHID_send(buf);
		_delay_ms(5);
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
#ifdef PLATFORM_ikea
		scan_matrix();
#endif
	} else {
		++num;
	}
	USB_set_endpoint(prev_endp);
}


