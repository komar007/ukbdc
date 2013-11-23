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

#include "usb_config.h"
#include <avr/pgmspace.h>

const struct endpoint_config PROGMEM endpoint_configs[NUM_ENDPOINTS] = {
	{.num = 0,
		.type = EP_TYPE_CONTROL,
		.config = EP_SIZE_32 | EP_SINGLE_BUFFER,
		_BV(RXSTPE)},
	{.num = KEYBOARD_ENDPOINT,
		.type = EP_TYPE_INTERRUPT_IN,
		.config = EP_SIZE_32 | EP_SINGLE_BUFFER,
		.int_flags = 0x00},
	{.num = RAWHID_TX_ENDPOINT,
		.type = EP_TYPE_INTERRUPT_IN,
#ifdef PLATFORM_ghpad
		.config = EP_SIZE_32 | EP_SINGLE_BUFFER,
#else
		.config = EP_SIZE_64 | EP_DOUBLE_BUFFER,
#endif
		.int_flags = 0x00},
	{.num = RAWHID_RX_ENDPOINT,
		.type = EP_TYPE_INTERRUPT_OUT,
#ifdef PLATFORM_ghpad
		.config = EP_SIZE_32 | EP_SINGLE_BUFFER,
#else
		.config = EP_SIZE_64 | EP_DOUBLE_BUFFER,
#endif
		.int_flags = _BV(RXOUTE)}
};

#include "hid.h"
#include "rawhid.h"
#include "rawhid_protocol.h"
#include "main.h"
const struct interface_request_handler PROGMEM
iface_req_handlers[NUM_INTERFACE_REQUEST_HANDLERS] = {
	{.iface_num = KEYBOARD_INTERFACE,
		.f = &HID_handle_control_request},
	{.iface_num = RAWHID_INTERFACE,
		.f = &RAWHID_handle_control_request}
};
const struct endpoint_interrupt_handler PROGMEM
endpoint_int_handlers[NUM_ENDPOINT_INTERRUPT_HANDLERS] = {
	{.endpoint_num = RAWHID_RX_ENDPOINT,
		.f = &RAWHID_PROTOCOL_handle_packet}
};
