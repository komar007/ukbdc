#pragma once

#include "usb.h"

#define KEYBOARD_INTERFACE	0
#define KEYBOARD_ENDPOINT	3
#define KEYBOARD_SIZE		32

extern uint8_t keyboard_modifier_keys;
extern uint8_t keyboard_keys[30];


bool HID_handle_control_request(struct setup_packet*);
void HID_handle_sof();
void HID_commit_state();
