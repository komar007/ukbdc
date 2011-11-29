#pragma once

/* Do not include this file anywhere else than usb.c */

#define STR_MANUFACTURER	L"komar"
#define STR_PRODUCT		L"komar board 0.1"

#define VENDOR_ID		0x16C0
#define PRODUCT_ID		0x047C

#define ENDPOINT0_SIZE		32

/* to handler declarations */
#include "hid.h"

struct interface_request_handler iface_req_handlers[] = {
	{.iface_number = KEYBOARD_INTERFACE, .function = &HID_handle_control_request}
};

struct sof_handler sof_handlers[] = {
	{.function = &HID_handle_sof}
};
