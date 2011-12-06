#pragma once

#include "usb.h"

#define KEYBOARD_INTERFACE	0
#define KEYBOARD_ENDPOINT	1
#define KEYBOARD_SIZE		32


#define BOOT_PROTOCOL		0
#define REPORT_PROTOCOL		1

bool HID_handle_control_request(struct setup_packet*);
void HID_handle_sof();

bool HID_scancode_is_pressed(uint8_t code);
void HID_set_scancode_state(uint8_t code, bool state);
void HID_commit_state();
