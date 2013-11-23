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

#include "usb.h"
#include "usb_config.h"

#define RAWHID_USAGE_PAGE	0xFFAB	// recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE		0x0200	// recommended: 0x0100 to 0xFFFF

bool RAWHID_handle_control_request(struct setup_packet *s);
bool RAWHID_send(const void *buffer);
bool RAWHID_recv(void *buffer);
