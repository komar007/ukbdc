#include "hid.h"
#include "system.h"

/* The protocol the keyboard is using at the moment */
static volatile uint8_t keyboard_protocol = REPORT_PROTOCOL;

/* the idle configuration, how often we send the report to the
 * host (ms * 4) even when it hasn't changed */
static volatile uint16_t keyboard_idle_config=500;

/* countdown until idle timeout */
static volatile uint16_t keyboard_idle_countdown=500;

static volatile bool keyboard_send_now = false;

// 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
static volatile uint8_t keyboard_leds = 0;
static volatile bool leds_changed = false;

/* 256-bit HID report to send to the host */
static volatile uint8_t key_map[32] = {0};
static volatile uint8_t six_keys[6] = {0};

void HID_send_boot_report()
{
	/* Send byte 28 of key_map, which is the state of modifiers */
	USB_IN_write_byte(key_map[28]);
	/* send reserved byte (0) */
	USB_IN_write_byte(0x00);
	USB_IN_write_buffer((void*)six_keys, 6);
}

void HID_send_report()
{
	switch (keyboard_protocol) {
	case REPORT_PROTOCOL:
		USB_IN_write_buffer((void*)key_map, 32);
		break;
	case BOOT_PROTOCOL:
		HID_send_boot_report();
		break;
	default:
		break;
	}
}

/* [Callbacks section] ----------------------------------------------------- */

bool HID_handle_control_request(struct setup_packet *s)
{
	if (request_type(s, DIRECTION, DEVICE_TO_HOST)) {
		USB_wait_IN();
		switch (s->bRequest) {
		case HID_GET_REPORT:
			HID_send_report();
			break;
		case HID_GET_IDLE:
			USB_IN_write_byte(keyboard_idle_config);
			break;
		case HID_GET_PROTOCOL:
			USB_IN_write_byte(keyboard_protocol);
			break;
		default:
			return false;
		}
		USB_flush_IN();
	} else {
		switch(s->bRequest) {
		case HID_SET_REPORT:
			USB_wait_OUT();
			keyboard_leds = USB_OUT_read_byte();
			leds_changed = true;
			USB_flush_OUT();
			break;
		case HID_SET_IDLE:
			keyboard_idle_config = (s->wValue >> 8);
			keyboard_idle_countdown = keyboard_idle_config;
			break;
		case HID_SET_PROTOCOL:
			keyboard_protocol = s->wValue;
			break;
		default:
			return false;
		}
	}
	return true;
}

void HID_handle_sof(message_type_t __attribute__((unused)) type, void __attribute__((unused)) *data)
{
	if (!USB_get_configuration())
		return;
	bool should_send = keyboard_send_now ||
		(keyboard_idle_config != 0 && keyboard_idle_countdown == 0);
	if (!should_send)
		return;
	USB_set_endpoint(KEYBOARD_ENDPOINT);
	/* substitute data to be sent with new version if last buffer has not
	 * been sent */
	if (!USB_IN_ready())
		USB_kill_banks();
	HID_send_report();
	USB_flush_IN();
	keyboard_send_now = false;
	keyboard_idle_countdown = keyboard_idle_config;
}

/* [/Callbacks section] ---------------------------------------------------- */

/* [API section] ----------------------------------------------------------- */

void HID_init()
{
	SYSTEM_subscribe(USB_SOF, HID_handle_sof);
}

/* Checks if a key is pressed */
bool HID_scancode_is_pressed(uint8_t code)
{
	uint8_t byte_no = code / 8;
	uint8_t bit_no = code & 0x07;
	return key_map[byte_no] & _BV(bit_no);
}

void HID_set_scancode_state(uint8_t code, bool state)
{
	uint8_t byte_no = code / 8;
	uint8_t bit_no = code & 0x07;
	if (state == false)
		key_map[byte_no] &= ~_BV(bit_no);
	else
		key_map[byte_no] |= _BV(bit_no);
	/* The part below is not tested! */
	if (keyboard_protocol == BOOT_PROTOCOL) {
		if (state == true) {
			uint8_t pos = 0;
			for (; pos < 6 && six_keys[pos] != 0; ++pos)
				;
			if (pos < 6)
				six_keys[pos] = code;
		} else {
			uint8_t pos = 0;
			for (; pos < 6 && six_keys[pos] != code; ++pos)
				;
			if (pos < 6)
				six_keys[pos] = 0;
		}
	}
}

/* send the contents of keyboard_keys and keyboard_modifier_keys */
void HID_commit_state()
{
	keyboard_send_now = true;
}

uint8_t HID_get_leds()
{
	return keyboard_leds;
}

uint8_t HID_leds_changed()
{
	if (leds_changed) {
		leds_changed = false;
		return true;
	} else {
		return false;
	}
}

/* [/API section] ---------------------------------------------------------- */
