#include "hid.h"

// which modifier keys are currently pressed
// 1=left ctrl,    2=left shift,   4=left alt,    8=left gui
// 16=right ctrl, 32=right shift, 64=right alt, 128=right gui
uint8_t keyboard_modifier_keys=0;

// which keys are currently pressed, up to 6 keys may be down at once
uint8_t keyboard_keys[30]={0};

// protocol setting from the host.  We use exactly the same report
// either way, so this variable only stores the setting since we
// are required to be able to report which setting is in use.
static uint8_t keyboard_protocol=BOOT_PROTOCOL;

/* the idle configuration, how often we send the report to the
 * host (ms * 4) even when it hasn't changed */
uint16_t keyboard_idle_config=500;

/* countdown until idle timeout */
uint16_t keyboard_idle_countdown=500;

bool keyboard_send_now = false;

// 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
volatile uint8_t keyboard_leds=0;

void HID_send_report()
{
	USB_IN_write_byte(keyboard_modifier_keys);
	USB_IN_write_byte(0);
	USB_IN_write_buffer(keyboard_keys, 30);
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
			USB_ack_OUT();
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

void HID_handle_sof()
{
	bool should_send = keyboard_send_now ||
		(keyboard_idle_config != 0 && keyboard_idle_countdown == 0);
	if (!should_send)
		return;
	USB_set_endpoint(KEYBOARD_ENDPOINT);
	/* check if sending is possible on interrupt endpoint */
	if (bit_is_set(UEINTX, RWAL)) {
		HID_send_report();
		UEINTX &= ~_BV(FIFOCON);
		keyboard_send_now = false;
		keyboard_idle_countdown = keyboard_idle_config;
	}
}

/* [/Callbacks section] ---------------------------------------------------- */

/* [API section] ----------------------------------------------------------- */

/* send the contents of keyboard_keys and keyboard_modifier_keys */
void HID_commit_state()
{
	keyboard_send_now = true;
}

/* [/API section] ---------------------------------------------------------- */
