/* This file is part of ukbdc.
 *
 * ukbdc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ukbdc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ukbdc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "usb_config.h"

#define BOOT_PROTOCOL		0
#define REPORT_PROTOCOL		1

bool HID_handle_control_request(struct setup_packet*);
void HID_handle_sof();

void HID_init();
bool HID_scancode_is_pressed(uint8_t code);
void HID_set_scancode_state(uint8_t code, bool state);
uint8_t HID_get_leds();
void HID_commit_state();
uint8_t HID_leds_changed();
