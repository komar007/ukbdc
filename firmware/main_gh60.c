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

struct layout PROGMEM scan_codes = {
	.num_keys = 64,
	.num_layers = 4,
	.data = {
		{KESC},{K1},{K2},{K3},{K4},{K5},{K6},{K7},{K8},{K9},{K0},{KMINUS},{KEQUAL},{KBACKSPACE},
		{KTAB},{KQ},{KW},{KE},{KR},{KT},{KY},{KU},{KI},{KO},{KP},{KLEFT_BRACE},{KRIGHT_BRACE},{KBACKSLASH},
		{KCAPS_LOCK},{KA},{KS},{KD},{KF},{KG},{KH},{KJ},{KK},{KL},{KSEMICOLON},{KQUOTE},{KBACKSLASH},{KENTER},
		{KLEFT_SHIFT, ACT(DOWN, REL), 1, 0},{0},{KZ},{KX},{KC},{KV},{KB},{KN},{KM},{KCOMMA},{KPERIOD},{KSLASH},{0},{KRIGHT_SHIFT, ACT(DOWN, REL), 1, 0},
		{KLEFT_CTRL},{0, ACT(DOWN, ABS), 2, 0},{KLEFT_ALT},{KSPACE},{KRIGHT_ALT},{KRIGHT_GUI},{KRIGHT_GUI},{KRIGHT_CTRL},

		{KTILDE},{K1},{K2},{K3},{K4},{K5},{K6},{K7},{K8},{K9},{K0},{KMINUS},{KEQUAL},{KBACKSPACE},
		{KTAB},{KQ},{KW},{KE},{KR},{KT},{KY},{KU},{KI},{KO},{KP},{KLEFT_BRACE},{KRIGHT_BRACE},{KBACKSLASH},
		{KCAPS_LOCK},{KA},{KS},{KD},{KF},{KG},{KH},{KJ},{KK},{KL},{KSEMICOLON},{KQUOTE},{KBACKSLASH},{KENTER},
		{KLEFT_SHIFT, ACT(UP, REL), 0, -1},{0},{KZ},{KX},{KC},{KV},{KB},{KN},{KM},{KCOMMA},{KPERIOD},{KSLASH},{0},{KRIGHT_SHIFT, ACT(UP, REL), 0, -1},
		{KLEFT_CTRL},{0, ACT(DOWN, ABS), 3, 0},{KLEFT_ALT},{KSPACE},{KRIGHT_ALT},{KRIGHT_GUI},{KRIGHT_GUI},{KRIGHT_CTRL},

		{KTILDE},{KF1},{KF2},{KF3},{KF4},{KF5},{KF6},{KF7},{KF8},{KF9},{KF10},{KF11},{KF12},{KDELETE},
		{KTAB},{KQ},{KUP},{KE},{KR},{KT},{KY},{KU},{KINSERT},{KO},{KPAGE_UP},{KLEFT_BRACE},{KRIGHT_BRACE},{KBACKSLASH},
		{KCAPS_LOCK},{KLEFT},{KDOWN},{KRIGHT},{KF},{KG},{KLEFT},{KDOWN},{KUP},{KRIGHT},{KPAGE_DOWN},{KQUOTE},{KBACKSLASH},{KENTER},
		{KLEFT_SHIFT, ACT(DOWN, REL), 1, 0},{0},{KZ},{KX},{KC},{KV},{KB},{KN},{KM},{KCOMMA},{KPERIOD},{KSLASH},{0},{KRIGHT_SHIFT, ACT(DOWN, REL), 1, 0},
		{KLEFT_CTRL},{0, ACT(UP, ABS), 0, 0},{KLEFT_ALT},{KSPACE},{KRIGHT_ALT},{KRIGHT_GUI},{KRIGHT_GUI},{KRIGHT_CTRL},

		{KESC},{KF1},{KF2},{KF3},{KF4},{KF5},{KF6},{KF7},{KF8},{KF9},{KF10},{KF11},{KF12},{KDELETE},
		{KTAB},{KQ},{KUP},{KE},{KR},{KT},{KY},{KU},{KINSERT},{KO},{KPAGE_UP},{KLEFT_BRACE},{KRIGHT_BRACE},{KBACKSLASH},
		{KCAPS_LOCK},{KLEFT},{KDOWN},{KRIGHT},{KF},{KG},{KLEFT},{KDOWN},{KUP},{KRIGHT},{KPAGE_DOWN},{KQUOTE},{KBACKSLASH},{KENTER},
		{KLEFT_SHIFT, ACT(UP, REL), 0, -1},{0},{KZ},{KX},{KC},{KV},{KB},{KN},{KM},{KCOMMA},{KPERIOD},{KSLASH},{0},{KRIGHT_SHIFT, ACT(UP, REL), 0, -1},
		{KLEFT_CTRL},{0, ACT(UP, ABS), 0, 1},{KLEFT_ALT},{KSPACE},{KRIGHT_ALT},{KRIGHT_GUI},{KRIGHT_GUI},{KRIGHT_CTRL},
	}
};

#define LED 19

int main(void)
{
	clock_prescale_set(clock_div_1);

	IO_config(LED, OUTPUT);
	IO_set(LED, true);

	for (int i = 5; i < 19; ++i) {
		IO_config(i, INPUT);
		IO_set(i, true);
	}
	for (int i = 0; i < 5; ++i) {
		IO_config(i, INPUT);
		IO_set(i, false); // no pull-up
	}

	LAYOUT_set(&scan_codes);
	LAYOUT_set_callback(&HID_set_scancode_state);

	MATRIX_init(5, rows, 14, cols, (const uint8_t*)matrix, &LAYOUT_set_key_state);

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
		if (HID_get_leds() & 0x02)
			IO_set(LED, false);
		else
			IO_set(LED, true);
	} else {
		++num;
	}
	USB_set_endpoint(prev_endp);
}
