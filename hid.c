#include "hid.h"

// which modifier keys are currently pressed
// 1=left ctrl,    2=left shift,   4=left alt,    8=left gui
// 16=right ctrl, 32=right shift, 64=right alt, 128=right gui
uint8_t keyboard_modifier_keys=0;

// which keys are currently pressed, up to 6 keys may be down at once
uint8_t keyboard_keys[30]={0,0,0,0,0,0};

// protocol setting from the host.  We use exactly the same report
// either way, so this variable only stores the setting since we
// are required to be able to report which setting is in use.
static uint8_t keyboard_protocol=0;

// the idle configuration, how often we send the report to the
// host (ms * 4) even when it hasn't changed
uint8_t keyboard_idle_config=125;

// count until idle timeout
uint8_t keyboard_idle_count=0;

// 1=num lock, 2=caps lock, 4=scroll lock, 8=compose, 16=kana
volatile uint8_t keyboard_leds=0;

void handle_hid_control_request(struct setup_packet *s)
{
	if (request_type(s, MASK_REQ_DIR, DEVICE_TO_HOST)) {
		USB_wait_IN();
		switch (s->bRequest) {
		case HID_GET_REPORT:
			USB_IN_write_byte(keyboard_modifier_keys);
			USB_IN_write_byte(0);
			for (int i=0; i<30; i++)
				USB_IN_write_byte(keyboard_keys[i]);
			break;
		case HID_GET_IDLE:
			USB_IN_write_byte(keyboard_idle_config);
			break;
		case HID_GET_PROTOCOL:
			USB_IN_write_byte(keyboard_protocol);
			break;
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
			keyboard_idle_count = 0;
			break;
		case HID_SET_PROTOCOL:
			keyboard_protocol = s->wValue;
			break;
		}
	}
}
