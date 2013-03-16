#pragma once

#include "usb_config.h"

#define BOOT_PROTOCOL		0
#define REPORT_PROTOCOL		1

bool HID_handle_control_request(struct setup_packet*);
void HID_handle_sof();

bool HID_scancode_is_pressed(uint8_t code);
void HID_set_scancode_state(uint8_t code, bool state);
uint8_t HID_get_leds();
void HID_commit_state();
uint8_t HID_leds_changed();
